cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

find_package(CPPUTILS2 REQUIRED)

add_executable(example src/example.cpp)
target_link_libraries(example PRIVATE CPPUTILS2::cpputils2)
