#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// curl stuff
#include <curl/curl.h>

// protobuf stuff
#include "proto/helloworld.pb-c.h"

#define PROJECT_NAME "curl_grpc"

#define PREFIX_LENGTH 5

int *dec_to_bin(long num, size_t len);
long bin_to_dec(int *bin, size_t len);
size_t handle_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("message is required\n");
    return 1;
  }

  Helloworld__HelloRequest request = HELLOWORLD__HELLO_REQUEST__INIT;
  uint8_t *buf;
  size_t len;

  request.name = argv[1];
  len = helloworld__hello_request__get_packed_size(&request);
  buf = malloc(PREFIX_LENGTH + len);

  buf[0] = 0;
  int *bin = dec_to_bin(len, 32);
  for (int idx = 0; idx < 4; idx++) {
    long num = bin_to_dec(bin + idx * 8, 8);
    buf[4 - idx] = num;
  }
  free(bin);

  helloworld__hello_request__pack(&request, buf + PREFIX_LENGTH);

  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (!curl) {
    fprintf(stderr, "unable to initialize curl");
    curl_global_cleanup();
    return 1;
  }

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/grpc+proto");

  curl_easy_setopt(curl, CURLOPT_URL,
                   "http://localhost:50051/helloworld.Greeter/SayHello");
  curl_easy_setopt(curl, CURLOPT_HTTP_VERSION,
                   CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len + 5);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  free(buf);

  return 0;
}

int *dec_to_bin(long num, size_t len) {
  int *result = malloc(sizeof(int) * len);
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
  size_t realsize = size * nmemb;

  Helloworld__HelloReply *response =
      helloworld__hello_reply__unpack(NULL, realsize - PREFIX_LENGTH, ptr+PREFIX_LENGTH);
  if (response == NULL) {
    fprintf(stderr, "error unpacking incoming message\n");
    exit(1);
  }
  printf("response msg: %s\n", response->msg);
  helloworld__hello_reply__free_unpacked(response, NULL);

  return realsize;
}
