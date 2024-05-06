cmake_minimum_required(VERSION 3.22)
# set project name
project(JaggerExtention CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/jagger-2023-02-18/src) 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/jagger-2023-02-18) 
# include_directories(${Boost_INCLUDE_DIRS})

# file(GLOB SRC_LM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/kenlm/lm/*.cc)
# file(GLOB SRC_UTIL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/kenlm/util/*.cc)

# add_subdirectory(kenlm)

add_executable(${PROJECT_NAME}
              # ./jagger-2023-02-18/src/jagger.h
              # ./jagger-2023-02-18/src/ccedar_core.h
              ./jagger-2023-02-18/src/jagger.cc
              ./main.cc

              # ${SRC_LM_FILES}
              # ${SRC_UTIL_FILES}
            )

target_compile_options(${PROJECT_NAME} PUBLIC -g -O3 -Wall )
# target_link_libraries(${PROJECT_NAME} PUBLIC kenlm_util kenlm)

