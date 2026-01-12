#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <vector>

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

  google::protobuf::compiler::MultiFileErrorCollector *error_collector = new SimpleErrorCollector();
  google::protobuf::compiler::DiskSourceTree *source_tree = new google::protobuf::compiler::DiskSourceTree();
  source_tree->MapPath("", ".");

  google::protobuf::compiler::Importer *importer = new google::protobuf::compiler::Importer(source_tree, error_collector);
  const google::protobuf::FileDescriptor *fd = importer->Import(argv[1]);

  google::protobuf::util::TypeResolver *type_resolver = google::protobuf::util::NewTypeResolverForDescriptorPool("type.googleapis.com", fd->pool());
  std::string payload;
  google::protobuf::util::Status status = google::protobuf::util::JsonToBinaryString(type_resolver, "type.googleapis.com/helloworld.HelloRequest", argv[2], &payload);
  if (!status.ok()) {
    std::cout << "fail to parse json to proto message\n";
    return 1;
}

  size_t len = payload.length();
  std::vector<uint8_t> buf = {0, 0, 0, 0, 0};
  buf.insert(buf.end(), payload.begin(), payload.end());

  int *bin = dec_to_bin(len, 32);
  for (int idx = 0; idx < 4; idx++) {
    long num = bin_to_dec(bin + idx * 8, 8);
    buf[4 - idx] = num;
  }
  free(bin);

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
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf.data());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len + PREFIX_LENGTH);
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/grpc+proto");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fd->pool());

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));

  curl_easy_cleanup(curl);
  curl_global_cleanup();

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
  std::string payload_json;
  google::protobuf::util::TypeResolver *type_resolver = google::protobuf::util::NewTypeResolverForDescriptorPool("type.googleapis.com", (google::protobuf::DescriptorPool *) userdata);
  google::protobuf::util::Status status_second = google::protobuf::util::BinaryToJsonString(type_resolver, "type.googleapis.com/helloworld.HelloReply", ptr+PREFIX_LENGTH, &payload_json);

  if (!status_second.ok())
    std::cout << "fail to parse message to json\n";
  else
    std::cout << "response: " << payload_json << "\n";

  return size * nmemb;
}
