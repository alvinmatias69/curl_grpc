/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: helloworld.proto */

#ifndef PROTOBUF_C_helloworld_2eproto__INCLUDED
#define PROTOBUF_C_helloworld_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protobuf-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1005001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protobuf-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protobuf-c.
#endif


typedef struct Helloworld__HelloRequest Helloworld__HelloRequest;
typedef struct Helloworld__HelloReply Helloworld__HelloReply;


/* --- enums --- */


/* --- messages --- */

struct  Helloworld__HelloRequest
{
  ProtobufCMessage base;
  char *name;
};
#define HELLOWORLD__HELLO_REQUEST__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&helloworld__hello_request__descriptor) \
    , (char *)protobuf_c_empty_string }


struct  Helloworld__HelloReply
{
  ProtobufCMessage base;
  char *msg;
};
#define HELLOWORLD__HELLO_REPLY__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&helloworld__hello_reply__descriptor) \
    , (char *)protobuf_c_empty_string }


/* Helloworld__HelloRequest methods */
void   helloworld__hello_request__init
                     (Helloworld__HelloRequest         *message);
size_t helloworld__hello_request__get_packed_size
                     (const Helloworld__HelloRequest   *message);
size_t helloworld__hello_request__pack
                     (const Helloworld__HelloRequest   *message,
                      uint8_t             *out);
size_t helloworld__hello_request__pack_to_buffer
                     (const Helloworld__HelloRequest   *message,
                      ProtobufCBuffer     *buffer);
Helloworld__HelloRequest *
       helloworld__hello_request__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   helloworld__hello_request__free_unpacked
                     (Helloworld__HelloRequest *message,
                      ProtobufCAllocator *allocator);
/* Helloworld__HelloReply methods */
void   helloworld__hello_reply__init
                     (Helloworld__HelloReply         *message);
size_t helloworld__hello_reply__get_packed_size
                     (const Helloworld__HelloReply   *message);
size_t helloworld__hello_reply__pack
                     (const Helloworld__HelloReply   *message,
                      uint8_t             *out);
size_t helloworld__hello_reply__pack_to_buffer
                     (const Helloworld__HelloReply   *message,
                      ProtobufCBuffer     *buffer);
Helloworld__HelloReply *
       helloworld__hello_reply__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   helloworld__hello_reply__free_unpacked
                     (Helloworld__HelloReply *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Helloworld__HelloRequest_Closure)
                 (const Helloworld__HelloRequest *message,
                  void *closure_data);
typedef void (*Helloworld__HelloReply_Closure)
                 (const Helloworld__HelloReply *message,
                  void *closure_data);

/* --- services --- */

typedef struct Helloworld__Greeter_Service Helloworld__Greeter_Service;
struct Helloworld__Greeter_Service
{
  ProtobufCService base;
  void (*say_hello)(Helloworld__Greeter_Service *service,
                    const Helloworld__HelloRequest *input,
                    Helloworld__HelloReply_Closure closure,
                    void *closure_data);
};
typedef void (*Helloworld__Greeter_ServiceDestroy)(Helloworld__Greeter_Service *);
void helloworld__greeter__init (Helloworld__Greeter_Service *service,
                                Helloworld__Greeter_ServiceDestroy destroy);
#define HELLOWORLD__GREETER__BASE_INIT \
    { &helloworld__greeter__descriptor, protobuf_c_service_invoke_internal, NULL }
#define HELLOWORLD__GREETER__INIT(function_prefix__) \
    { HELLOWORLD__GREETER__BASE_INIT,\
      function_prefix__ ## say_hello  }
void helloworld__greeter__say_hello(ProtobufCService *service,
                                    const Helloworld__HelloRequest *input,
                                    Helloworld__HelloReply_Closure closure,
                                    void *closure_data);

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor helloworld__hello_request__descriptor;
extern const ProtobufCMessageDescriptor helloworld__hello_reply__descriptor;
extern const ProtobufCServiceDescriptor helloworld__greeter__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_helloworld_2eproto__INCLUDED */
