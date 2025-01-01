cmake_minimum_required(VERSION 3.14)
project(cpputils2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options()
endif()

set(LOG_VERBOSITY SPDLOG_LEVEL_DEBUG CACHE STRING "Log level verbosity")

set(CMAKE_CXX_STANDARD 23)

set(ENABLE_TEST ON CACHE BOOL "Enable project testing")
set(ENABLE_EXAMPLES ON CACHE BOOL "Enable examples building")



set(COMPILATION_FLAGS -ggdb -Wall -Wextra -Wconversion -Wno-sign-conversion -Wpedantic -Wnon-virtual-dtor -Woverloaded-virtual -Wreorder -Wuninitialized -fsanitize=undefined -DSPDLOG_ACTIVE_LEVEL=${LOG_VERBOSITY}  CACHE STRING "Compilation flags")
set(LINK_SANITIZER ubsan CACHE STRING "Link to sanitizer")
set(LINK_FLAGS -fsanitize=undefined CACHE STRING "Linker flags for sanitizer")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

separate_arguments(COMPILATION_FLAGS UNIX_COMMAND "${COMPILATION_FLAGS}")


include(GNUInstallDirs)

if (${ENABLE_TEST})
include(CTest)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    message("Unit test enabled")
    enable_testing()
endif()




add_subdirectory(src)

if (${ENABLE_EXAMPLES})
  add_subdirectory(examples)
endif()

