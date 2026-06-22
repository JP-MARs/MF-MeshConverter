project(MFmesh CXX)
#出力先はベース
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MFmeshBase_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MFmeshBase_BINARY_DIR})

#message("BaseLIBOUT = ${MFmeshBase_BINARY_DIR}" )
#message("DIR = ${CMAKE_SOURCE_DIR}" )

file(GLOB SRC_FILES Converter/*.cpp ConverterRead/*.cpp FileReadWrite/*.cpp *.cpp)

#コンパイル設定
add_library(MFMeshConv
  STATIC ${SRC_FILES}
)

# setting for old intel
# set for Intel OneAPI
if(CMAKE_CXX_COMPILER MATCHES "icpx")
 message("Intel New Compiler use in SparseSolv !") 
 #set(CMAKE_AR "/opt/intel/oneapi/compiler/latest/bin/compiler/llvm-ar")
 #set(CMAKE_NM "/opt/intel/oneapi/compiler/latest/bin/compiler/llvm-nm")
 target_include_directories(MFMeshConv 
   PUBLIC ${CMAKE_SOURCE_DIR}/include  ${CMAKE_SOURCE_DIR}/thirdparty  ${CMAKE_SOURCE_DIR}
 )
 target_compile_options(MFMeshConv PUBLIC -std=c++14 -O3 -xhost)
 target_link_libraries(MFMeshConv PUBLIC -qmkl -ipo)
#
# gcc setting
else()
  find_package(OpenMP REQUIRED)
  target_include_directories(MFMeshConv 
    PUBLIC ${CMAKE_SOURCE_DIR}/include  ${CMAKE_SOURCE_DIR}/thirdparty  ${CMAKE_SOURCE_DIR}
  )
  target_compile_options(MFMeshConv PUBLIC -std=c++14 -O3)
  target_link_libraries(MFMeshConv PUBLIC OpenMP::OpenMP_CXX)
endif()
