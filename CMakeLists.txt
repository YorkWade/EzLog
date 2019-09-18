#windows cmake  -G visual studio 10 2010 ..

cmake_minimum_required (VERSION 2.6)
set(ALLOW_DUPLICATE_CUSTOM_TARGETS TRUE)

project (EzLog)

#include header to vs
file(GLOB EzLog_lib_head_files "./src/*.h")

aux_source_directory("./src" EzLog_lib_src_files)

add_executable(EzLog ${EzLog_lib_src_files} ${EzLog_lib_head_files})