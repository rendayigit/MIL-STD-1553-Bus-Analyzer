#pragma once

#include <climits>
#include <filesystem>
#include <linux/limits.h>
#include <stdemace.h>
#include <string>
#include <unistd.h>

constexpr int ACE_ERROR_BUFFER_SIZE = 80;

constexpr int HEX_BYTE = 16;
constexpr int RT_SA_MAX_COUNT = 32;

constexpr int TOP_BAR_COMP_HEIGHT = 30;

constexpr int DATA_OCTET = 8;

enum BcMode { BC_TO_RT, RT_TO_BC, RT_TO_RT };

static std::string getExecutableDirectory() {
  char execPathStr[PATH_MAX]; // NOLINT

  // Get the path of the executable by reading /proc/self/exe symbolic link
  ssize_t len = readlink("/proc/self/exe", execPathStr, PATH_MAX - 1);
  if (len != -1) {
    execPathStr[len] = '\0'; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
  }

  // Get the path of the executable's parent directory using filesystem operations
  std::filesystem::path execPath(execPathStr);
  std::filesystem::path parentPath = execPath.parent_path();

  // Navigate to the project's bin directory and resolve final path
  std::string pathStr = parentPath.string();

  // Find the last directory in the path (e.g., "/bin" etc.) for proper navigation
  std::string dirName = parentPath.filename().string();
  size_t dirPos = pathStr.rfind("/" + dirName);

  if (dirPos != std::string::npos) {
    // Keep everything up to and including the directory name for consistent path structure
    pathStr = pathStr.substr(0, dirPos + 1 + dirName.length());
  }

  // Return path with trailing slash for convenient concatenation
  return (pathStr + '/');
}

static std::string getStatus(S16BIT statusCode) {
  char buf[ACE_ERROR_BUFFER_SIZE]; // NOLINT
  aceErrorStr(statusCode, buf, ACE_ERROR_BUFFER_SIZE);
  return buf;
}