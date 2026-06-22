project(MFmeshConv CXX)
#出力先はベース
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MFmeshBase_BINARY_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MFmeshBase_BINARY_DIR})
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MFmeshBase_BINARY_DIR})


file(GLOB SRC_FILES *.cpp)

#コンパイル設定
add_executable(mf_meshconverter.out
  ${SRC_FILES}
)

# set for Intel OneAPI
if(CMAKE_CXX_COMPILER MATCHES "icpx")
 message("Intel New Compiler use in mf_meshconverter.out  !") 
 target_include_directories(mf_meshconverter.out 
   PUBLIC ${CMAKE_SOURCE_DIR}/include  ${CMAKE_SOURCE_DIR}/thirdparty  ${CMAKE_SOURCE_DIR}
 )
 target_compile_options(mf_meshconverter.out PUBLIC -std=c++14 -O3 -xhost)
 target_link_libraries(mf_meshconverter.out PUBLIC -qmkl -ipo MFMeshConv)
#
# gcc setting
else()
  find_package(OpenMP REQUIRED)
  target_include_directories(mf_meshconverter.out 
    PUBLIC ${CMAKE_SOURCE_DIR}/include  ${CMAKE_SOURCE_DIR}/thirdparty  ${CMAKE_SOURCE_DIR}
  )
  target_compile_options(mf_meshconverter.out PUBLIC -std=c++14 -O3)
  target_link_libraries(mf_meshconverter.out PUBLIC OpenMP::OpenMP_CXX MFMeshConv)
endif()
