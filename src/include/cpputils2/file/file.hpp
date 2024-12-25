#pragma once

#include "cpputils2/common/types.hpp"

#include <fstream>
#include <optional>
#include <string>

namespace CppUtils2
{

  inline bool file_exists(const std::string &name)
  {
    std::ifstream f(name.c_str());
    return f.good();
  }

  inline Result write_whole_file(const std::string &path, const std::string &content)
  {
    std::ofstream output_fs(path);
    if (!output_fs.good())
    {
      return Result::RET_ERROR;
    }
    output_fs << content;
    return Result::RET_OK;
  }

  inline std::optional<std::string> read_whole_file(const std::string &path)
  {
    std::ifstream input_fs(path);
    if (!input_fs.good())
    {
      return std::nullopt;
    }
    std::string content((std::istreambuf_iterator<char>(input_fs)), (std::istreambuf_iterator<char>()));
    return content;
  }

} // namespace CppUtils2