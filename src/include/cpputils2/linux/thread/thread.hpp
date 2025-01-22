/**
 * @file thread.hpp
 * @brief Various thread utilities
 *
 *
 */

#pragma once

#include "cpputils2/common/types.hpp"

#include <pthread.h>
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <cassert>
#include <expected>
#include <thread>

namespace CppUtils2
{
    struct ThreadConfig
    {
        int policy;
        int priority;
    };

    struct RealTimeThreadConfig : public ThreadConfig
    {
        RealTimeThreadConfig()
        {
            policy = SCHED_FIFO;
            priority = 99;
        }
    };

    struct BestEffortThreadConfig : public ThreadConfig
    {
        BestEffortThreadConfig()
        {
            policy = SCHED_OTHER;
            priority = 0;
        }
    };

    Result set_thread_sched_policy(std::thread &thread, const int policy, const int priority)
    {
        struct sched_param param;
        param.sched_priority = priority;

        // ensure that native_handler() is a pthread_t
        assert(typeid(thread.native_handle()) == typeid(pthread_t));

        int ret = pthread_setschedparam(thread.native_handle(), policy, &param);
        if (ret != 0)
        {
            return Result::RET_ERROR;
        }

        return Result::RET_OK;
    }

    Result set_thread_sched_policy(std::thread &thread, const ThreadConfig &config)
    {
        return set_thread_sched_policy(thread, config.policy, config.priority);
    }

    std::expected<ThreadConfig, Result> get_thread_sched_policy(std::thread &thread)
    {
        int policy;
        struct sched_param param;

        // ensure that native_handler() is a pthread_t
        assert(typeid(thread.native_handle()) == typeid(pthread_t));
        ThreadConfig config;
        int ret = pthread_getschedparam(thread.native_handle(), &config.policy, &param);
        config.priority = param.sched_priority;
        Result ret_result = Result::RET_ERROR;
        if (ret != 0)
        {
            return std::unexpected(ret_result);
        }

        return config;
    }

}