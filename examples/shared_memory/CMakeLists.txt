project(example_shared_memory  VERSION 1.0.0 LANGUAGES CXX)


add_executable(shared_memory src/main.cpp)

target_link_libraries(shared_memory PRIVATE CPPUTILS2::cpputils2 spdlog::spdlog)