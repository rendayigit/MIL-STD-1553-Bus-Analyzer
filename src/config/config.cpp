#include "config.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

const std::string CONFIG_FILE_PATH = std::getenv("HOME") + std::string("/.bus-analyzer-config.json"); // NOLINT
constexpr int MAX_LIST_SIZE = 5;

void Config::createConfigFileIfNotExists() {
  // Check if file exists
  if (std::filesystem::exists(CONFIG_FILE_PATH)) {
    return; // File already exists, nothing to do
  }

  // Create default config
  nlohmann::json defaultConfig = {{"Bus_Monitor_Default_Device_Number", "0"},
                                  {"Bus_Monitor_UI_Recent_Line_Count", "1000"}, // NOLINT
                                  {"Bus_Controller_Default_Device_Number", "1"},
                                  {"Remote_Terminal_Default_Device_Number", "2"}};

  // Write default config to file
  std::ofstream configFile(CONFIG_FILE_PATH);
  if (configFile.is_open()) {
    configFile << defaultConfig.dump(2);
    configFile.close();
  } else {
    std::cerr << "Could not create config file at: " << CONFIG_FILE_PATH << std::endl;
  }
}

void Config::updateKeyInConfig(const std::string &key, const std::string &value) {
  nlohmann::json config;
  std::ifstream configFile(CONFIG_FILE_PATH);

  if (configFile.is_open()) {
    try {
      // Read the config file
      configFile >> config;
      configFile.close();

      // Update the key
      config[key] = value;

      // Write the updated config back to the file
      std::ofstream outConfigFile(CONFIG_FILE_PATH, std::ios::trunc);

      if (outConfigFile.is_open()) {
        outConfigFile << config.dump(2);
        outConfigFile.close();
      } else {
        std::cerr << "Could not open config file for writing: " << CONFIG_FILE_PATH << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << "Error writing to config file: " << e.what() << std::endl;
    }
  } else {
    std::cerr << "Could not open config file for reading: " << CONFIG_FILE_PATH << std::endl;
  }
}

void Config::addValueToListInConfig(const std::string &key, const std::string &value) {
  nlohmann::json config;
  std::ifstream configFile(CONFIG_FILE_PATH);

  if (configFile.is_open()) {
    try {
      // Read the config file
      configFile >> config;
      configFile.close();

      // If key doesn't exist, create it as an array
      if (not config.contains(key)) {
        config[key] = nlohmann::json::array();
      }

      // Remove the value if it already exists
      auto &list = config[key];
      list.erase(std::remove(list.begin(), list.end(), value), list.end());

      // Add the value to the beginning of the list
      list.insert(list.begin(), value);

      // Limit the number of items stored (remove from the end)
      while (list.size() > MAX_LIST_SIZE) {
        list.erase(list.end() - 1);
      }

      // Write the updated config back to the file
      std::ofstream outConfigFile(CONFIG_FILE_PATH, std::ios::trunc);

      if (outConfigFile.is_open()) {
        outConfigFile << config.dump(2);
        outConfigFile.close();
      } else {
        std::cerr << "Could not open config file for writing: " << CONFIG_FILE_PATH << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << "Error writing to config file: " << e.what() << std::endl;
    }
  } else {
    std::cerr << "Could not open config file for reading: " << CONFIG_FILE_PATH << std::endl;
  }
}

void Config::removeValueFromListInConfig(const std::string &key, const std::string &value) {
  nlohmann::json config;
  std::ifstream configFile(CONFIG_FILE_PATH);

  if (configFile.is_open()) {
    try {
      // Read the config file
      configFile >> config;
      configFile.close();

      // Remove the value from list
      auto &list = config[key];
      list.erase(std::remove(list.begin(), list.end(), value), list.end());

      // Write the updated config back to the file
      std::ofstream outConfigFile(CONFIG_FILE_PATH, std::ios::trunc);

      if (outConfigFile.is_open()) {
        outConfigFile << config.dump(2);
        outConfigFile.close();
      } else {
        std::cerr << "Could not open config file for writing: " << CONFIG_FILE_PATH << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << "Error writing to config file: " << e.what() << std::endl;
    }
  } else {
    std::cerr << "Could not open config file for reading: " << CONFIG_FILE_PATH << std::endl;
  }
}

std::vector<std::string> Config::getListItemsFromConfig(const std::string &key) {
  nlohmann::json config;
  std::ifstream configFile(CONFIG_FILE_PATH);
  std::vector<std::string> items;

  if (configFile.is_open()) {
    try {
      // Read the config file
      configFile >> config;
      configFile.close();

      // Extract the list items
      if (config.contains(key) and config[key].is_array()) {
        for (const auto &item : config[key]) {
          items.push_back(item.get<std::string>());
        }
      } else {
        std::cerr << "Key missing: '" << key << "'" << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << "Error reading list from config: " << e.what() << std::endl;
    }
  } else {
    std::cerr << "Could not open config file for reading: " << CONFIG_FILE_PATH << std::endl;
  }

  return items;
}

std::string Config::getValueFromConfig(const std::string &key) {
  nlohmann::json config;
  std::ifstream configFile(CONFIG_FILE_PATH);
  std::string value;

  if (configFile.is_open()) {
    try {
      // Read the config file
      configFile >> config;
      configFile.close();

      // Extract the value
      if (config.contains(key) and config[key].is_string()) {
        value = config[key].get<std::string>();
      } else {
        std::cerr << "Key missing or not a string: '" << key << "'" << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << "Error reading value from config: " << e.what() << std::endl;
    }
  } else {
    std::cerr << "Could not open config file for reading: " << CONFIG_FILE_PATH << std::endl;
  }

  return value;
}