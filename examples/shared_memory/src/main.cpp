
#include <cpputils2/cpputils2.hpp>

#ifdef _WIN32
#include <cpputils2/win/shm/shm.hpp>
#else
#include <cpputils2/linux/shm/shm.hpp>
#endif

#include <iostream>
#include <spdlog/spdlog.h>

const int32_t expected = 42;
const std::string file_name("test_shm");

int main(int argc, char **argv)
{
  SPDLOG_INFO("cpputils2 version {}", CppUtils2::VERSION);
  SPDLOG_INFO("Test shared memory");

  CppUtils2::Shm shm(file_name);
  auto ret = shm.allocate(sizeof(int32_t));
  if (ret == CppUtils2::Result::RET_ERROR)
  {
    SPDLOG_ERROR("Error");
    return 1;
  }

  void *ptr = shm.get_raw_ptr();
  if (ptr == nullptr)
  {
    SPDLOG_ERROR("Error");
    return 1;
  }
  int32_t *ptr_int = reinterpret_cast<int32_t *>(ptr);
  SPDLOG_INFO("ptr_int: {}", *ptr_int);
  *ptr_int = expected;

  int32_t val = *ptr_int;

  CppUtils2::Shm shm2("test_shm");
  ret = shm2.open_existing(sizeof(int32_t));
  if (ret == CppUtils2::Result::RET_ERROR)
  {
    SPDLOG_ERROR("Error");
    return 1;
  }
  ptr = shm2.get_raw_ptr();
  ptr_int = reinterpret_cast<int32_t *>(ptr);

  SPDLOG_INFO("ptr_int: {}", *ptr_int);

  val = *ptr_int;

  SPDLOG_INFO("val: {} expected: {}", val, expected);
  if (val != expected)
  {
    SPDLOG_ERROR("Error");
    return 1;
  }
  shm2.close();
  shm.close();
  shm.unlink();

  return 0;
}