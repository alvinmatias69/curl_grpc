project('curl_grpc', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : true)

deps = [
    dependency('libcurl', version: '>=7.50.0'),
    m_dep
    ]

sources = ['curl_grpc.c']

exe = executable('curl_grpc', sources,
  install : true, dependencies : deps)

test('basic', exe)
