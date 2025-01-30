#pragma once

#include <climits>
#include <filesystem>
#include <fstream>
#include <linux/limits.h>
#include <nlohmann/json.hpp>
#include <stdemace.h>
#include <string>
#include <unistd.h>

#include "logger.hpp"

constexpr int ACE_ERROR_BUFFER_SIZE = 80;

constexpr int HEX_BYTE = 16;
constexpr int RT_SA_MAX_COUNT = 32;

static std::string getExecutableDirectory();

const std::string CONFIG_PATH = getExecutableDirectory() + "../config.json";

static std::string getStatus(S16BIT statusCode);

static int getDefaultDeviceNumber();

static int getMaxRecentLineCount();
inline std::string getExecutableDirectory() {
  char result[PATH_MAX]; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                         // cppcoreguidelines-avoid-c-arrays)

  // Get the path of the executable
  (void)readlink("/proc/self/exe", result, PATH_MAX);

  // Get the path of the executable's parent directory
  std::filesystem::path exePath(result);

  // Get the path of the executable's parent directory
  return (exePath.parent_path().string() + '/');
}

inline std::string getStatus(S16BIT statusCode) {
  char buf[ACE_ERROR_BUFFER_SIZE]; // NOLINT(hicpp-avoid-c-arrays,
                                   // modernize-avoid-c-arrays,
                                   // cppcoreguidelines-avoid-c-arrays)

  aceErrorStr(statusCode, buf, ACE_ERROR_BUFFER_SIZE);

  return buf;
}

inline int getDefaultDeviceNumber() {
  nlohmann::json config;

  // Load the JSON file
  std::ifstream configFile(CONFIG_PATH);
  if (not configFile.is_open()) {
    Logger::error("Could not open the config file: " + CONFIG_PATH);
    return -1;
  }

  // Parse the JSON file
  try {
    configFile >> config; // Parse the JSON file
  } catch (const nlohmann::json::parse_error &e) {
    Logger::error("JSON parse error: " + std::string(e.what()));
    return -1;
  }

  // Check if the key exists and is of the correct type
  if (config.contains("DEFAULT_DEVICE_NUMBER") && config["DEFAULT_DEVICE_NUMBER"].is_number_integer()) {
    int deviceNumber = config["DEFAULT_DEVICE_NUMBER"].get<int>();
    return deviceNumber;
  }

  Logger::error("Key 'DEFAULT_DEVICE_NUMBER' not found or is not an integer.");
  return -1;
}

inline int getMaxRecentLineCount() {
  nlohmann::json config;

  // Load the JSON file
  std::ifstream configFile(CONFIG_PATH);
  if (not configFile.is_open()) {
    Logger::error("Could not open the config file: " + CONFIG_PATH);
    return -1;
  }

  // Parse the JSON file
  try {
    configFile >> config; // Parse the JSON file
  } catch (const nlohmann::json::parse_error &e) {
    Logger::error("JSON parse error: " + std::string(e.what()));
    return -1;
  }

  // Check if the key exists and is of the correct type
  if (config.contains("UI_RECENT_LINE_COUNT") && config["UI_RECENT_LINE_COUNT"].is_number_integer()) {
    int deviceNumber = config["UI_RECENT_LINE_COUNT"].get<int>();
    return deviceNumber;
  }

  Logger::error("Key 'UI_RECENT_LINE_COUNT' not found or is not an integer.");
  return -1;
}
