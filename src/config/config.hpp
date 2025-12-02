#pragma once

#include <nlohmann/json.hpp>
#include <vector>

class Config {
public:
  static void createConfigFileIfNotExists();
  static void updateKeyInConfig(const std::string &key, const std::string &value);
  static void addValueToListInConfig(const std::string &key, const std::string &value);
  static void removeValueFromListInConfig(const std::string &key, const std::string &value);
  static std::vector<std::string> getListItemsFromConfig(const std::string &key);
  static std::string getValueFromConfig(const std::string &key);

private:
  Config() = default;
};