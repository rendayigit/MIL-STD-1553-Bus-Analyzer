#ifndef BM_HPP
#define BM_HPP

#include "logger/logger.hpp"
#include "message.hpp"
#include "stdemace.h"
#include <array>
#include <functional>
#include <string>
#include <thread>

class BM {
public:
  BM();
  ~BM();

  S16BIT startBm(int devNum);
  S16BIT stopBm();
  
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

private:
  static Message getMessage(MSGSTRUCT *msg);
  void monitor();

  std::function<void(const std::string &)> m_updateMessages;
  std::function<void(char bus, int rt, int sa, bool state)> m_updateSaState;

  int m_devNum;
  int m_monitorPollThreadSleepMs;

  bool m_loop;
  bool m_filter;

  char m_filteredBus;
  int m_filteredRt;
  int m_filteredSa;

  std::thread m_monitorThread;
  Logger m_logger;
};

#endif // BM_HPP