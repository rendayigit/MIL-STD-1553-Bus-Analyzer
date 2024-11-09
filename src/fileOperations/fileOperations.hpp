#include <iostream>
#include <string>

#include <climits>
#include <filesystem>
#include <linux/limits.h>
#include <unistd.h>

#ifndef FILE_OPERATIONS
#define FILE_OPERATIONS

class FileOperations {
public:
  /**
   * @brief Get the FileOperations instance
   *
   * @return FileOperations singleton instance
   */
  static FileOperations &getInstance() {
    static FileOperations instance;
    return instance;
  }

  /**
   * @brief Get the executable directory
   *
   * @return The executable directory
   */
  std::string getExecutableDirectory() { return m_executableDirectory; }

private:
  FileOperations() {
    char result[PATH_MAX]; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                           // cppcoreguidelines-avoid-c-arrays)

    // Get the path of the executable
    (void)readlink("/proc/self/exe", result, PATH_MAX);

    // Get the path of the executable's parent directory
    std::filesystem::path exePath(result);

    // Get the path of the executable's parent directory
    m_executableDirectory = exePath.parent_path().string() + '/';
  }

  std::string m_executableDirectory;
};

#endif // FILE_OPERATIONS