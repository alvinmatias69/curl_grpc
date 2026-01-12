#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>

#include <string>
#include <iostream>

// curl stuff
#include <curl/curl.h>

// protobuf stuff
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/util/type_resolver.h>
#include <google/protobuf/util/type_resolver_util.h>
#include <google/protobuf/util/json_util.h>

#define PROJECT_NAME "curl_grpc"

#define PREFIX_LENGTH 5

int *dec_to_bin(long num, size_t len);
long bin_to_dec(int *bin, size_t len);
size_t handle_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

class SimpleErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector {
public:
  void AddError(const std::string & filename, int line, int column, const std::string & message) override {
    std::cout << "error processing " << filename << " on line: " << line << " column: " << column << " message: " << message << "\n";
  }

  void AddWarning(const std::string & filename, int line, int column, const std::string & message) override {
    std::cout << "warning processing " << filename << " on line: " << line << " column: " << column << " message: " << message << "\n";
  }
};


int main(int argc, char **argv) {
  if (argc < 2) {
    printf("proto is required\n");
    return 1;
  }

  if (argc < 3) {
    printf("payload is required\n");
    return 1;
  }

  std::cout << argv[2] << "\n";

  google::protobuf::compiler::MultiFileErrorCollector *error_collector = new SimpleErrorCollector();
  google::protobuf::compiler::DiskSourceTree *source_tree = new google::protobuf::compiler::DiskSourceTree();
  source_tree->MapPath("", ".");

  google::protobuf::compiler::Importer *importer = new google::protobuf::compiler::Importer(source_tree, error_collector);
  const google::protobuf::FileDescriptor *fd = importer->Import(argv[1]);
  std::cout << "sucess load protofile: " << fd->name() << "\n";

  google::protobuf::util::TypeResolver *type_resolver = google::protobuf::util::NewTypeResolverForDescriptorPool("type.googleapis.com", fd->pool());
  std::string payload;
  google::protobuf::util::Status status = google::protobuf::util::JsonToBinaryString(type_resolver, "type.googleapis.com/helloworld.HelloRequest", argv[2], &payload);
  if (!status.ok())
    std::cout << "fail to parse json to message: " << status.error_message() << "\n";

  // std::cout << "payload length: " << payload.length() << "\n";

  // std::string payload_json;
  // google::protobuf::util::Status status_second = google::protobuf::util::BinaryToJsonString(type_resolver, "type.googleapis.com/helloworld.HelloRequest", payload, &payload_json);
  // if (!status_second.ok())
  //   std::cout << "fail to parse message to json: " << status.error_message() << "\n";

  // std::cout << "json payload: " << payload_json << "\n";

  // char command[255];
  // sprintf(command, "protoc --encode=helloworld.HelloRequest %s > encoded_payload", argv[1]);
  // FILE *encode_pipe = popen(command, "w");
  // fputs(argv[2], encode_pipe);
  // pclose(encode_pipe);

  // struct stat st;
  // stat("encoded_payload", &st);
  // size_t len = st.st_size;
  size_t len = payload.length();
  // uint8_t *buf;
  // buf = (uint8_t *) malloc(PREFIX_LENGTH);

  // buf[0] = 0;
  std::string buf;
  int *bin = dec_to_bin(len, 32);
  for (int idx = 0; idx < 4; idx++) {
    long num = bin_to_dec(bin + idx * 8, 8);
    // buf[4 - idx] = num;
    buf.insert(0, 1, num);
  }
  std::cout << "buf: " << buf << "\n";
  // free(bin);
  // strncpy((char *)buf+len, payload.c_str(), len);
  std::string payload_buf = buf + payload;

  // FILE *encoded_payload = fopen("encoded_payload", "rb");
  // fread(buf+PREFIX_LENGTH, len,1, encoded_payload);
  // fclose(encoded_payload);

  std::cout << "payload: " << payload_buf << "\n";
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (!curl) {
    fprintf(stderr, "unable to initialize curl");
    curl_global_cleanup();
    return 1;
  }

  curl_easy_setopt(curl, CURLOPT_URL,
                   "http://localhost:50051/helloworld.Greeter/SayHello");
  curl_easy_setopt(curl, CURLOPT_HTTP_VERSION,
                   CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_buf.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len + PREFIX_LENGTH);
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/grpc+proto");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, argv[1]);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  std::cout << "here\n";
  // free(buf);

  return 0;
}

int *dec_to_bin(long num, size_t len) {
  int *result = (int *) malloc(sizeof(int) * len);
  for (size_t idx = 0; idx < len; idx++) {
    result[idx] = num % 2;
    num = num / 2;
  }
  return result;
}

long bin_to_dec(int *bin, size_t len) {
  long result = 0;
  for (size_t idx = 0; idx < len; idx++) {
    result = result + bin[idx] * pow(2, idx);
  }
  return result;
}

size_t handle_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  char command[255];
  sprintf(command, "protoc --decode=helloworld.HelloReply %s > response_decoded", (char *) userdata);
  FILE *decode_pipe = popen(command, "w");
  fputs(ptr+PREFIX_LENGTH, decode_pipe);
  pclose(decode_pipe);

  return size * nmemb;
}
