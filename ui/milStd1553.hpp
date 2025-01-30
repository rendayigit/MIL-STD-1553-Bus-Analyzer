#pragma once

#include "wx/treebase.h"
#include <array>
#include <string>
#include <wx/treectrl.h>

constexpr int BUS_COUNT = 2;
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

  class Bus : public MilStd1553Item {
  public:
    std::array<Rt, RT_COUNT> rtList;
  };

public:
  static MilStd1553 &getInstance() {
    static MilStd1553 instance;
    return instance;
  }

  std::array<Bus, BUS_COUNT> busList;

private:
  MilStd1553() {
    busList.at(0).setName("BUS A");
    busList.at(1).setName("BUS B");

    for (auto &bus : busList) {
      for (size_t i = 0; i < RT_COUNT; ++i) {
        bus.rtList.at(i).setName("RT " + std::to_string(i));

        for (size_t j = 0; j < SA_COUNT; ++j) {
          bus.rtList.at(i).saList.at(j).setName("SA " + std::to_string(j));
        }
      }
    }
  }
};