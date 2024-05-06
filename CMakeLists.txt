cmake_minimum_required(VERSION 3.22)
# set project name
project(JaggerExtension CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/jagger-2023-02-18/src) 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/jagger-2023-02-18) 
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/jagger-2023-02-18/model/kwdlc)


file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/jagger-2023-02-18/model/kwdlc/patterns)    

add_executable(${PROJECT_NAME}
              # ./jagger-2023-02-18/src/jagger.h
              # ./jagger-2023-02-18/src/ccedar_core.h
              ./jagger-2023-02-18/src/jagger.cc
              ./main.cc
              ./jagger-2023-02-18/src/ccedar_core.cc
            )

target_compile_options(${PROJECT_NAME} PUBLIC -g -O3 -Wall )

