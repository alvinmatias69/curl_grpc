project('curl_grpc', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3'])

cc = meson.get_compiler('cpp')

m_dep = cc.find_library('m', required : true)

protobuf = dependency('protobuf',
    version : '>= 2.6.0',
    native : true)

libprotoc = cc.find_library('protoc',
    dirs : protobuf.get_pkgconfig_variable('libdir'))


deps = [
    dependency('libcurl', version: '>=7.50.0'),
    m_dep,
    protobuf,
    libprotoc,
    ]

sources = ['curl_grpc.cc']

exe = executable('curl_grpc', sources, dependencies : deps)

test('basic', exe)
