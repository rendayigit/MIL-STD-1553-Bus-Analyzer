#ifndef BC_HPP
#define BC_HPP

#include <array>
#include <stdemace.h>
#include <string>

#include "common.hpp"
#include "configData.hpp"

class BC {
public:
  // TODO: make bm singleton as well
  static BC &getInstance() {
    static BC instance;
    return instance;
  }

  ConfigData getConfigData() const { return *m_configData; }

  S16BIT startBc(S16BIT devNum);
  S16BIT stopBc();
  S16BIT bcToRt(int rt, int sa, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> data);
  S16BIT rtToBc(int rt, int sa, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> *data);
  S16BIT rtToRt(int rtTx, int saTx, int rtRx, int saRx, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> *data);
  S16BIT configRun();

  void setCommandFilePath(const std::string &commandFilePath) { m_commandFilePath = commandFilePath; }

  int getDevNum() const { return m_devNum; }

private:
  BC();
  ~BC();

  ConfigData *m_configData;
  U16BIT m_messageBuffer[RT_SA_MAX_COUNT]; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                                           // cppcoreguidelines-avoid-c-arrays)
  std::string m_commandFilePath;
  S16BIT m_devNum;
};

#endif // BC_HPP