cmake_minimum_required(VERSION 3.14)

Include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.x
)

FetchContent_MakeAvailable(spdlog)

add_subdirectory(shared_memory)

if (WIN32)

else()
add_subdirectory(linux_deadline)
endif()
