cmake_minimum_required(VERSION 3.13)
project(cpputils2  VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options()
endif()


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# find_package(common REQUIRED)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/cpputils2/cpputils2.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/include/cpputils2/cpputils2.hpp)


set(SOURCES_AND_HEADERS 
    include/cpputils2/cpputils2.hpp
    include/cpputils2/common/types.hpp
    include/cpputils2/trigger/trigger.hpp
    include/cpputils2/file/file.hpp
    include/cpputils2/net/socket/isocket.hpp
)

if (WIN32)

list(APPEND SOURCES_AND_HEADERS 
    include/cpputils2/win/shm/shm.hpp
    include/cpputils2/win/net/socket/socket.hpp
    include/cpputils2/win/net/socket/tcpsocketclient.hpp
    include/cpputils2/win/net/socket/tcpsocketserver.hpp)

else()

    list(APPEND SOURCES_AND_HEADERS
    include/cpputils2/linux/net/socket/tcpsocketclient.hpp
    include/cpputils2/linux/net/socket/tcpsocketserver.hpp
    include/cpputils2/linux/net/socket/udpsocketserver.hpp
    include/cpputils2/linux/net/socket/udpsocketclient.hpp
    include/cpputils2/linux/net/socket/socket.hpp
    include/cpputils2/linux/net/socket/udsclient.hpp
    include/cpputils2/linux/net/socket/udsserver.hpp
    include/cpputils2/linux/shm/shm.hpp
    include/cpputils2/linux/futex/futex.hpp
    include/cpputils2/linux/futex/shared_futex.hpp
    include/cpputils2/linux/thread/thread.hpp
    )

endif()

add_library(cpputils2 INTERFACE 
    ${SOURCES_AND_HEADERS}
)

#Set target properties
target_include_directories( cpputils2
    INTERFACE
        $<INSTALL_INTERFACE:.>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(CPPUTILS2::cpputils2 ALIAS cpputils2)

# target_link_libraries(cpputils2 INTERFACE spdlog::spdlog Threads::Threads)


include(CMakePackageConfigHelpers)

set(PROJECT_CONFIG_FILE  "${PROJECT_BINARY_DIR}/CPPUTILS2Config.cmake")



write_basic_package_version_file(
    CPPUTILS2ConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Cpputils2Config.cmake.in
            CPPUTILS2Config.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpputils2
)


install (TARGETS cpputils2 EXPORT cpputils2-targets
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
         INCLUDES DESTINATION include/cpputils2)

export(EXPORT cpputils2-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/CPPUTILS2Targets.cmake"
  NAMESPACE CPPUTILS2::
)


install(EXPORT cpputils2-targets
        FILE CPPUTILS2Targets.cmake
        NAMESPACE 
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpputils2
)






install (DIRECTORY include/cpputils2 
         DESTINATION include)





install(FILES  ${CMAKE_CURRENT_BINARY_DIR}/CPPUTILS2Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/CPPUTILS2ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpputils2
)

if (${ENABLE_TEST})
add_executable(test_cpputils2 test/main.cpp test/cpputils2_tests.cpp)
target_include_directories( test_cpputils2
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(TEST_LIBS GTest::gtest GTest::gtest_main  spdlog::spdlog)

if (WIN32)
    list(APPEND TEST_LIBS ws2_32)
else()
    list(APPEND TEST_LIBS rt)
endif()

target_link_libraries(test_cpputils2 PRIVATE CPPUTILS2::cpputils2 ${TEST_LIBS})
include(CTest)

#add_test(CppUtils2Test test_cpputils2 --gtest_output=xml)
include(GoogleTest)
gtest_discover_tests(test_cpputils2)

endif()
