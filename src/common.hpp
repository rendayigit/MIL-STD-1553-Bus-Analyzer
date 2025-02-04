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

static int getDefaultDeviceNumber() {
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

  // Check if the Bus_Monitor key exists and contains Default_Device_Number
  if (config.contains("Bus_Monitor") and config["Bus_Monitor"].contains("Default_Device_Number") and
      config["Bus_Monitor"]["Default_Device_Number"].is_number_integer()) {
    return config["Bus_Monitor"]["Default_Device_Number"].get<int>();
  }

  Logger::error("Key 'Default_Device_Number' not found in 'Bus_Monitor' or is not an integer.");
  return -1;
}

static int getMaxRecentLineCount() {
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

  // Check if the Bus_Monitor key exists and contains UI_Recent_Line_Count
  if (config.contains("Bus_Monitor") and config["Bus_Monitor"].contains("UI_Recent_Line_Count") and
      config["Bus_Monitor"]["UI_Recent_Line_Count"].is_number_integer()) {
    return config["Bus_Monitor"]["UI_Recent_Line_Count"].get<int>();
  }

  Logger::error("Key 'UI_Recent_Line_Count' not found in 'Bus_Monitor' or is not an integer.");
  return -1;
}
