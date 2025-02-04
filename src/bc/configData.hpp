#ifndef CONFIG_DATA
#define CONFIG_DATA

#include "common.hpp"
#include <array>
#include <string>
#include <utility>

class ConfigData {
public:
  explicit ConfigData(std::string deviceNum, std::string bus, int rtRx, int saRx, int rtTx, int saTx, int wc,
                      int bcMode, std::array<std::string, RT_SA_MAX_COUNT> &data)
      : m_deviceNum(std::move(deviceNum)), m_bus(std::move(bus)), m_rtRx(rtRx), m_saRx(saRx), m_rtTx(rtTx),
        m_saTx(saTx), m_wc(wc), m_bcMode(bcMode), m_data(std::move(data)) {}

  std::string getDeviceNum() const { return m_deviceNum; }
  std::string getBus() const { return m_bus; }
  int getRtRx() const { return m_rtRx; }
  int getSaRx() const { return m_saRx; }
  int getRtTx() const { return m_rtTx; }
  int getSaTx() const { return m_saTx; }
  int getWc() const { return m_wc; }
  int getBcMode() const { return m_bcMode; }
  std::array<std::string, RT_SA_MAX_COUNT> getData() const { return m_data; }

private:
  std::string m_deviceNum;
  std::string m_bus;
  int m_rtRx;
  int m_saRx;
  int m_rtTx;
  int m_saTx;
  int m_wc;
  int m_bcMode;
  std::array<std::string, RT_SA_MAX_COUNT> m_data;
};

#endif // CONFIG_DATA