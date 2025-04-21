#pragma once

#include <array>
#include <string>

#include <wx/treebase.h>
#include <wx/treectrl.h>

constexpr int RT_COUNT = 32;
constexpr int SA_COUNT = 32;

class MilStd1553 {
private:
  class MilStd1553Item {
  public:
    std::string getName() const { return m_name; }
    void setName(const std::string &name) { m_name = name; }

    wxTreeItemId getTreeObject() const { return m_treeObject; }
    void setTreeObject(const wxTreeItemId &treeObject) { m_treeObject = treeObject; }

  private:
    std::string m_name;
    wxTreeItemId m_treeObject;
  };

  class Sa : public MilStd1553Item {};

  class Rt : public MilStd1553Item {
  public:
    std::array<Sa, SA_COUNT> saList;
  };

public:
  static MilStd1553 &getInstance() {
    static MilStd1553 instance;
    return instance;
  }

  std::array<Rt, RT_COUNT> rtList;

private:
  MilStd1553() {

    for (size_t i = 0; i < RT_COUNT; ++i) {
      rtList.at(i).setName("RT " + std::to_string(i));

      for (size_t j = 0; j < SA_COUNT; ++j) {
        rtList.at(i).saList.at(j).setName("SA " + std::to_string(j));
      }
    }
  }
};