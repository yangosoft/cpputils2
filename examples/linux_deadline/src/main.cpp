
#include <cpputils2/cpputils2.hpp>

#include <cpputils2/linux/sched/sched.hpp>

#include <iostream>
#include <spdlog/spdlog.h>

const int32_t expected = 42;
const std::string file_name("test_shm");

int main(int argc, char **argv)
{
  SPDLOG_INFO("cpputils2 version {}", CppUtils2::VERSION);
  SPDLOG_INFO("Test deadline Linux scheduler");
  CppUtils2::sched_attr attr;
  attr.size = sizeof(CppUtils2::sched_attr);
  attr.sched_policy = SCHED_DEADLINE;
  attr.sched_runtime = 200000000;
  attr.sched_deadline = attr.sched_period = 1000000000;
  attr.sched_flags = SCHED_RESET_ON_FORK;

  auto ret = CppUtils2::set_self_attributes(&attr);

  if (!ret.has_value())
  {
    auto error = ret.error();
    SPDLOG_ERROR("Error setting attributes. Error code: {}, errno: {}", error, errno);
    return -1;
  }

  while (true)
  {
    SPDLOG_DEBUG("Looping");
    sched_yield();
  }

  return 0;
}