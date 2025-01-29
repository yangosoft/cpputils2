
#include <cpputils2/cpputils2.hpp>
#include <cpputils2/linux/sched/sched.hpp>

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <spdlog/spdlog.h>

#include <linux/sched.h>

const int32_t expected = 42;
const std::string file_name("test_shm");

std::atomic_flag run_thread = ATOMIC_FLAG_INIT;

void signalHandler(int signum)
{
  if (signum == SIGXCPU)
  {
    SPDLOG_INFO("Caught signal SIGXCPU. We are overruning the deadline");
    run_thread.clear();
  }
  else if (signum == SIGINT)
  {
    run_thread.clear();
  }
}

int main(int argc, char **argv)
{
  SPDLOG_INFO("cpputils2 version {}", CppUtils2::VERSION);
  SPDLOG_INFO("Test deadline Linux scheduler");
  CppUtils2::sched_attr attr;
  attr.size = sizeof(CppUtils2::sched_attr);
  attr.sched_policy = SCHED_DEADLINE;
  attr.sched_priority = 0;

  attr.sched_runtime = 200'000'000;
  attr.sched_deadline = 500'000'000;
  attr.sched_period = 500'000'000; // 1s
  attr.sched_flags = SCHED_FLAG_RESET_ON_FORK | SCHED_FLAG_RECLAIM | SCHED_FLAG_DL_OVERRUN;
  signal(SIGXCPU, signalHandler);
  signal(SIGINT, signalHandler);

  run_thread.test_and_set();

  auto ret = CppUtils2::set_self_attributes(&attr);

  if (!ret.has_value())
  {
    auto error = ret.error();
    SPDLOG_ERROR("Error setting attributes. Error code: {}, errno: {}", error, errno);
    return -1;
  }

  auto start = std::chrono::high_resolution_clock::now();

  bool first = true;
  uint64_t mean_time_ns = 0;
  uint64_t count = 0;

  while (run_thread.test())
  {
    if (first)
    {
      first = false;
    }
    else
    {
      auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
      std::cout << "Elapsed: " << elapsed << "ns\n";
      start = std::chrono::high_resolution_clock::now();
      mean_time_ns += elapsed;
      count++;
    }
    sched_yield();
  }

  SPDLOG_INFO("Exiting. Mean activation time {}ns", mean_time_ns / count);
  return 0;
}