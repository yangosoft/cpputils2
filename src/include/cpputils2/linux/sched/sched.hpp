#pragma once

#include "cpputils2/common/types.hpp"

#include <cassert>
#include <cstdint>
#include <expected>
#include <pthread.h>
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <span>

namespace CppUtils2
{

    // (yangosoft) As man 2 page says, this is a generic structure for scheduling attributes
    struct sched_attr
    {
        uint32_t size;               /* Size of this structure */
        uint32_t sched_policy = 0;   /* Policy (SCHED_*) */
        uint64_t sched_flags = 0;    /* Flags */
        int32_t sched_nice = 0;      /* Nice value (SCHED_OTHER,
                                  SCHED_BATCH) */
        uint32_t sched_priority = 0; /* Static priority (SCHED_FIFO,
                               SCHED_RR) */
        /* Remaining fields are for SCHED_DEADLINE */
        uint64_t sched_runtime = 0;
        uint64_t sched_deadline = 0;
        uint64_t sched_period = 0;

        sched_attr() : size(sizeof(sched_attr)) {}
    };

    int32_t sched_getattr(pid_t pid, sched_attr *attr, unsigned int size, unsigned int flags = 0)
    {
        return syscall(SYS_sched_getattr, pid, attr, size, flags);
    }

    int32_t sched_setattr(pid_t pid, const sched_attr *attr, uint32_t flags = 0)
    {
        return syscall(SYS_sched_setattr, pid, attr, flags);
    }

    std::expected<Result, int32_t> set_self_attributes(const sched_attr *attr)
    {
        pid_t me = getpid();
        int flags = 0;

        int32_t ret = sched_setattr(me, attr, flags);

        if (ret != 0)
        {
            return std::unexpected(ret);
        }

        return Result::RET_OK;
    }

    std::expected<Result, int32_t> set_process_core_affinity(pid_t pid, const cpu_set_t *mask)
    {
        int ret = sched_setaffinity(pid, sizeof(cpu_set_t), mask);
        if (ret != 0)
        {
            return std::unexpected(errno);
        }
        return Result::RET_OK;
    }

    std::expected<Result, int32_t> set_self_core_affinity(const cpu_set_t *mask)
    {
        return set_process_core_affinity(getpid(), mask);
    }

    std::expected<Result, int32_t> set_process_core_affinity(pid_t pid, const std::span<uint32_t, std::dynamic_extent> &mask)
    {

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        for (auto core : mask)
        {
            CPU_SET(core, &cpuset);
        }

        return set_process_core_affinity(pid, &cpuset);
    }

    std::expected<Result, int32_t> set_self_core_affinity(const std::span<uint32_t, std::dynamic_extent> &mask)
    {
        return set_process_core_affinity(getpid(), mask);
    }

} // namespace CppUtils2