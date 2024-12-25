#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace CppUtils2
{

  using FnCallback = std::function<void()>;
  class TriggerWaitable
  {
  public:
    void set_callback(FnCallback fn_callback)
    {
      callback = fn_callback;
    }

    void notify()
    {
      std::unique_lock<std::mutex> lock(mtx_cv);
      ready = true;
      cv.notify_all();
    }

    void wait()
    {
      std::unique_lock<std::mutex> lock(mtx_cv);
      cv.wait(lock, [this]
              { return ready; });
      callback();
    }

  private:
    FnCallback callback;
    std::mutex mtx_cv;
    std::condition_variable cv;
    bool ready = false;
  };

} // namespace CppUtils2