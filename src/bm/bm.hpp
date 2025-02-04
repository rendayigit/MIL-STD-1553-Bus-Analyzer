#pragma once

#include <functional>
#include <stdemace.h>
#include <string>
#include <thread>

#include "message.hpp"

class BM {
public:
  BM();
  ~BM();

  S16BIT start(int devNum);
  S16BIT stop();

  void setUpdateMessages(const std::function<void(const std::string &)> &updateMessages) {
    m_updateMessages = updateMessages;
  }

  void setUpdateSaState(const std::function<void(char bus, int rt, int sa, bool state)> &updateRtSaList) {
    m_updateSaState = updateRtSaList;
  }

  int getDevNum() const { return m_devNum; }

  void setFilter(bool filter) { m_filter = filter; }
  void setFilteredBus(char filteredBus) { m_filteredBus = filteredBus; }
  void setFilteredRt(int filteredRt) { m_filteredRt = filteredRt; }
  void setFilteredSa(int filteredSa) { m_filteredSa = filteredSa; }

  bool isMonitoring() const { return m_isMonitoring; }
  bool isFiltered() const { return m_filter; }

private:
  static Message getMessage(MSGSTRUCT *msg);
  void monitor();

  std::function<void(const std::string &)> m_updateMessages;
  std::function<void(char bus, int rt, int sa, bool state)> m_updateSaState;

  int m_devNum;

  bool m_isMonitoring;
  bool m_filter;

  char m_filteredBus;
  int m_filteredRt;
  int m_filteredSa;

  std::thread m_monitorThread;
};