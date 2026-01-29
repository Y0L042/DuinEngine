#pragma once

#include <string>

namespace duin
{
class FileUtils
{
  public:
    static bool ReadFileIntoString(const std::string &filePath, std::string &string);
    static bool WriteStringIntoFile(const std::string &filePath, const std::string &string);

  private:
};
} // namespace duin
