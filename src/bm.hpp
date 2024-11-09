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

  S16BIT startBm(S16BIT devNum);
  S16BIT stopBm();

  void setUpdateFilter(const std::function<void(const std::string &)> &updateFilter) { m_updateFilter = updateFilter; }

  void setUpdateMessages(const std::function<void(const std::string &)> &updateMessages) {
    m_updateMessages = updateMessages;
  }

  void setUpdateSaState(const std::function<void(int rt, int sa, bool state)> &updateRtSaList) {
    m_updateSaState = updateRtSaList;
  }

  void setLoop(bool loop) { m_loop = loop; }

private:
  static Message getMessage(MSGSTRUCT *msg);
  void monitor();

  std::function<void(const std::string &)> m_updateFilter;
  std::function<void(const std::string &)> m_updateMessages;
  std::function<void(int rt, int sa, bool state)> m_updateSaState;

  S16BIT m_devNum;
  int m_uiTotalMessageBufferSize;
  int m_uiSingleMessageBulkSize;
  int m_monitorPollThreadSleepMs;

  bool m_loop;

  std::thread m_monitorThread;
  Logger m_logger;
};

#endif // BM_HPP