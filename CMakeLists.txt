cmake_minimum_required(VERSION 3.8)
project(SK_strumieniowanie_dzwieku)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp functions.cpp functions.h)
add_executable(SK_strumieniowanie_dzwieku ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} pthread)