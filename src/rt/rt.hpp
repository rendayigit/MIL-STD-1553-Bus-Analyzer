#ifndef RT_HPP
#define RT_HPP

#include <functional>
#include <stdemace.h>

#include "common.hpp"

class RT {
public:
  static RT &getInstance() {
    static RT instance;
    return instance;
  }

  S16BIT start(int devNum);
  S16BIT stop();

  S16BIT setRt(int rt, int sa, const std::array<std::string, RT_SA_MAX_COUNT> &data);

  S16BIT enableRt(int rt);
  S16BIT disableRt(int rt);
  bool isRtActive(int rt);

  std::array<std::string, RT_SA_MAX_COUNT> getData(int rt, int sa);

  // TODO implement
  void setUpdateMessages(const std::function<void(const std::string &)> &updateMessages) {
    m_updateMessages = updateMessages;
  }

  bool isRunning() const { return m_isRunning; }

private:
  RT();
  ~RT();

  void setData(int rt, int sa, const std::array<std::string, RT_SA_MAX_COUNT> &data);

  std::function<void(const std::string &)> m_updateMessages; // TODO implement

  std::array<std::array<std::array<std::string, RT_SA_MAX_COUNT>, RT_SA_MAX_COUNT>, RT_SA_MAX_COUNT> m_data{};
  std::array<bool, RT_SA_MAX_COUNT> m_rtStatuses{};

  int m_devNum{};
  bool m_isRunning{};
};

#endif // RT_HPP