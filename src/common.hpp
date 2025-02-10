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

static std::string getExecutableDirectory() {
  char result[PATH_MAX]; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                         // cppcoreguidelines-avoid-c-arrays)

  // Get the path of the executable
  (void)readlink("/proc/self/exe", result, PATH_MAX); // NOLINT(bugprone-unused-return-value)

  // Get the path of the executable's parent directory
  std::filesystem::path exePath(result);

  // Get the path of the executable's parent directory
  return (exePath.parent_path().string() + '/');
}

const std::string CONFIG_PATH = getExecutableDirectory() + "../config.json";

static std::string getStatus(S16BIT statusCode) {
  char buf[ACE_ERROR_BUFFER_SIZE]; // NOLINT(hicpp-avoid-c-arrays,
                                   // modernize-avoid-c-arrays,
                                   // cppcoreguidelines-avoid-c-arrays)

  aceErrorStr(statusCode, buf, ACE_ERROR_BUFFER_SIZE);

  return buf;
}