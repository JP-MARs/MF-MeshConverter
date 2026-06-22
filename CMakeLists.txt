# CMake version set
cmake_minimum_required(VERSION 3.21)
# determine compiler
#set(CMAKE_CXX_COMPILER icpx)


#
if(CMAKE_CXX_COMPILER MATCHES "icpx")
 message("Intel new Compiler use !")
 set(CMAKE_AR "/opt/intel/oneapi/compiler/latest/bin/compiler/llvm-ar")
 set(CMAKE_NM "/opt/intel/oneapi/compiler/latest/bin/compiler/llvm-ar")
endif()
#

# set base dir
project(MFmeshBase CXX)

# set dir set
add_subdirectory(src)
add_subdirectory(src2)

message("Base = ${CMAKE_SOURCE_DIR}" )

