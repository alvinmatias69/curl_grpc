project('curl_grpc', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : true)

incdir = include_directories('proto')

deps = [
    dependency('libprotobuf-c', version: '>= 1.0.0'),
    dependency('libcurl', version: '>=7.50.0'),
    m_dep
    ]

sources = ['curl_grpc.c', './proto/helloworld.pb-c.c']

exe = executable('curl_grpc', sources,
  install : true, include_directories : incdir, dependencies : deps)

test('basic', exe)
