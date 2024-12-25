#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <utility>

namespace CppUtils2
{

  enum class Result : std::uint32_t
  {
    RET_ERROR = 0,
    RET_OK = 1
  };

} // namespace CppUtils2