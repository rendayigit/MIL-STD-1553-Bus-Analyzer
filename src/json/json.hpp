#include <boost/property_tree/json_parser.hpp>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifndef JSON_HPP
#define JSON_HPP

namespace pt = boost::property_tree;

class Json {
public:
  /**
   * @brief Construct a new Json object
   *
   * @param jsonFile The path to the json file
   * @throws std::runtime_error If the json file cannot be read
   */
  explicit Json(const std::string &jsonFile) : m_jsonFile(jsonFile) {
    try {
      // Read the json file and store the root node in `m_root`
      pt::read_json(jsonFile, m_root);
    } catch (std::exception const &e) {
      throw std::runtime_error("Cannot read " + jsonFile + " : " + e.what());
    }
  }

  /**
   * @brief Get the given node from the json file using the given key
   *
   * @param key The key of the node
   * @return The the node
   */
  Json getNode(const std::string &key) {
    // Push the provided key to the key history
    m_keyHistory.push_back(key);

    return Json(m_jsonFile, m_root.get_child(key), m_keyHistory);
  }

  /**
   * @brief Get the value of the given node
   *
   * @param t The type of the value
   * @return The value
   */
  template <typename t> t getValue() { return m_root.get_value<t>(); }

  /**
   * @brief Get the key of the node
   *
   * @return Key of the node
   */
  std::string getKey() { return m_keyHistory.back(); }

  /**
   * @brief Get the path of the node
   *
   * @return The path of the node
   */
  std::vector<std::string> getNodePath() { return m_keyHistory; }

  /**
   * @brief Get the Json node at the given index
   *
   * @param index The index of the node. The index can be positive or negative. Negative index will start from the end
   * of the array.
   * @throws std::invalid_argument If the Json node is not an array
   * @throws std::out_of_range If the index is out of range
   * @return The Json node at the given index
   */
  Json at(int index) {
    if (getSize() == 0) {
      throw std::invalid_argument("Json node is not an array.");
    }

    if ((getSize() <= index) or (getSize() + 1 <= abs(index))) {
      throw std::out_of_range("Index out of range.");
    }

    auto currentIndex = m_root.begin();

    if (index >= 0) {
      std::advance(currentIndex, index);
    } else {
      std::advance(currentIndex, getSize() + index);
    }

    m_keyHistory.push_back(currentIndex->first);

    return Json(m_jsonFile, currentIndex->second, m_keyHistory);
  }

  /**
   * @brief Get the size of the Json array
   *
   * @return The size of the Json array
   */
  int getSize() { return static_cast<int>(m_root.size()); }

  /**
   * @brief Set the value of the node
   *
   * @param value The value
   */
  template <typename t> void setValue(t value) {
    // Read the json file and store the root node in `m_root`
    pt::read_json(m_jsonFile, m_root);

    // The path of the node to be written
    std::string nodePath;

    // Construct the path to the node to be written
    for (auto &key : m_keyHistory) {
      nodePath += key + '.';
    }

    // Erase the last '.' to fix the path
    nodePath.erase(nodePath.length() - 1); // Erase last '.'

    // Write the value to the node
    m_root.put<t>(nodePath, value);

    // Overwrite the json file with the new value
    pt::write_json(m_jsonFile, m_root);
  }

private:
  // Hidden constructor for `getNode()` recursive function
  explicit Json(std::string &jsonFile, const pt::ptree &root, std::vector<std::string> keyHistory)
      : m_jsonFile(jsonFile), m_root(root), m_keyHistory(std::move(keyHistory)){};

  // The path to the json file
  std::string m_jsonFile;

  // The root of the json file
  pt::ptree m_root;

  // The history of the keys
  std::vector<std::string> m_keyHistory;
};

#endif // JSON_HPP