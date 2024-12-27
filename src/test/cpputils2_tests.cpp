#include <gtest/gtest.h>

#include "cpputils2/common/types.hpp"
#include "cpputils2/file/file.hpp"
#include "cpputils2/trigger/trigger.hpp"

#ifdef __linux__

#include "cpputils2/linux/futex/futex.hpp"
#include "cpputils2/linux/futex/shared_futex.hpp"
#include "cpputils2/linux/net/socket/tcpsocketclient.hpp"
#include "cpputils2/linux/net/socket/tcpsocketserver.hpp"
#include "cpputils2/linux/net/socket/udpsocketclient.hpp"
#include "cpputils2/linux/net/socket/udpsocketserver.hpp"
#include "cpputils2/linux/net/socket/udsclient.hpp"
#include "cpputils2/linux/net/socket/udsserver.hpp"
#include "cpputils2/linux/shm/shm.hpp"
#endif

#ifdef _WIN32
#include "cpputils2/win/shm/shm.hpp"
#endif

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>

namespace
{

#ifdef __linux__
  TEST(ExampleShm, TestInstance)
  {
    CppUtils2::Shm shm("/test_shm");
    auto ret = shm.allocate(sizeof(int32_t));
    EXPECT_NE(ret, CppUtils2::Result::RET_ERROR);

    void *ptr = shm.get_raw_ptr();
    int32_t *ptr_int = reinterpret_cast<int32_t *>(ptr);
    std::cout << "ptr_int: " << *ptr_int << std::endl;
    *ptr_int = 42;

    int32_t val = *ptr_int;
    EXPECT_EQ(val, 42);

    shm.close();
    shm.unlink();

    EXPECT_TRUE(true);
  }

  TEST(ExampleShm, TestExisting)
  {
    CppUtils2::Shm shm("/test_shm");
    auto ret = shm.allocate(sizeof(int32_t));
    EXPECT_NE(ret, CppUtils2::Result::RET_ERROR);

    void *ptr = shm.get_raw_ptr();
    int32_t *ptr_int = reinterpret_cast<int32_t *>(ptr);
    std::cout << "ptr_int: " << *ptr_int << std::endl;
    *ptr_int = 42;

    int32_t val = *ptr_int;
    EXPECT_EQ(val, 42);

    shm.close();

    CppUtils2::Shm shm2("/test_shm");
    ret = shm.open_existing(sizeof(int32_t));
    EXPECT_NE(ret, CppUtils2::Result::RET_ERROR);
    ptr = shm.get_raw_ptr();
    ptr_int = reinterpret_cast<int32_t *>(ptr);
    std::cout << "ptr_int: " << *ptr_int << std::endl;

    val = *ptr_int;
    EXPECT_EQ(val, 42);
    shm.close();
    shm.unlink();
  }

  TEST(ExampleFutex, TestInstance)
  {
    CppUtils2::Futex futex;
    futex.lock();
    futex.unlock();

    EXPECT_TRUE(true);
  }

  TEST(ExampleFutex, TestPost)
  {
    std::atomic_int32_t atom(0);
    CppUtils2::Futex futex(&atom);

    std::thread t([&futex]()
                  {
        auto ret = futex.wait(0);
        EXPECT_NE(ret, -1); });

    std::thread t2([&futex]()
                   {
        auto ret = futex.wait(0);
        EXPECT_NE(ret, -1); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    atom.store(2);
    auto ret = futex.post(2);
    EXPECT_NE(ret, -1);

    atom.store(0);
    ret = futex.post(2);
    EXPECT_NE(ret, -1);

    t.join();
    t2.join();
  }

  TEST(ExampleFutex, TestShared)
  {
    if (CppUtils2::file_exists("/dev/shm/test_futex"))
    {
      CppUtils2::unlink("/dev/shm/test_futex");
    }

    CppUtils2::Shm shm("/test_futex");
    shm.allocate(sizeof(int32_t));
    void *ptr = shm.get_raw_ptr();

    CppUtils2::SharedFutex futex;
    futex.init("/test_futex");
    futex.lock();
    futex.unlock();

    EXPECT_TRUE(true);
  }

#endif

#ifdef _WIN32
  TEST(ExampleSHM, TestSHM) {
	  CppUtils2::Shm shm(L"test_shm");
	  auto ret = shm.allocate(sizeof(int32_t));
	  EXPECT_NE(ret, CppUtils2::Result::RET_ERROR);
	  void* ptr = shm.get_raw_ptr();
      EXPECT_NE(ptr, nullptr);
	  int32_t* ptr_int = reinterpret_cast<int32_t*>(ptr);
	  std::cout << "ptr_int: " << *ptr_int << std::endl;
	  *ptr_int = 42;
	  int32_t val = *ptr_int;
	  EXPECT_EQ(val, 42);
	  shm.close();
	  shm.unlink();
	  EXPECT_TRUE(true);
  }

  TEST(ExampleShm, TestExisting)
  {
      CppUtils2::Shm shm(L"test_shm");
      auto ret = shm.allocate(sizeof(int32_t));
      EXPECT_NE(ret, CppUtils2::Result::RET_ERROR);

      void* ptr = shm.get_raw_ptr();
	  EXPECT_NE(ptr, nullptr);
      int32_t* ptr_int = reinterpret_cast<int32_t*>(ptr);
      std::cout << "ptr_int: " << *ptr_int << std::endl;
      *ptr_int = 42;

      int32_t val = *ptr_int;
      EXPECT_EQ(val, 42);

      

      CppUtils2::Shm shm2("test_shm");
      ret = shm2.open_existing(sizeof(int32_t));
      EXPECT_NE(ret, CppUtils2::Result::RET_ERROR);
      ptr = shm2.get_raw_ptr();
      ptr_int = reinterpret_cast<int32_t*>(ptr);
      std::cout << "ptr_int: " << *ptr_int << std::endl;

      val = *ptr_int;
      EXPECT_EQ(val, 42);
      shm2.close();
      shm.close();
      shm.unlink();
  }
#endif

  TEST(ExampleTrigger, TestTrigger)
  {
    CppUtils2::TriggerWaitable trigger;

    std::thread t([&trigger]()
                  {
                    bool ready = false;
                    trigger.set_callback([&ready]() { ready = true; });

                    trigger.wait();
                    EXPECT_TRUE(ready); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    trigger.notify();
    t.join();
    EXPECT_TRUE(true);
  }

}; // namespace
