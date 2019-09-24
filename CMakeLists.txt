#windows cmake  -G "Visual Studio 10 2010" ..

cmake_minimum_required (VERSION 2.6)
set(ALLOW_DUPLICATE_CUSTOM_TARGETS TRUE)

project (EzLog)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)

#release pdb
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

#include header to vs
file(GLOB EzLog_lib_head_files "./src/*.h")

aux_source_directory("./src" EzLog_lib_src_files)

add_executable(EzLog ${EzLog_lib_src_files} ${EzLog_lib_head_files})