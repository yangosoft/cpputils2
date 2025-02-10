cmake_minimum_required(VERSION 3.14)
project(example_linux_deadline  VERSION 1.0.0 LANGUAGES CXX)


if(NOT TARGET CPPUTILS2::cpputils2)
    message("Not target!")
    find_package(CPPUTILS2 REQUIRED)

    Include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.x
)

FetchContent_MakeAvailable(spdlog)
endif()


add_executable(linux_deadline src/main.cpp)

target_link_libraries(linux_deadline PRIVATE CPPUTILS2::cpputils2 spdlog::spdlog)
