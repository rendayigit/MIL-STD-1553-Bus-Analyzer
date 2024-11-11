#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "common.hpp"
#include <iomanip>
#include <string>
#include <utility>
#include <vector>

class Message {
public:
  explicit Message(U16BIT rt, U16BIT sa, U16BIT rtRx, U16BIT saRx, U16BIT wc, char bus, const char *type,
                   std::string time, const U16BIT *data, bool noRes)
      : m_rt(rt), m_sa(sa), m_rtRx(rtRx), m_saRx(saRx), m_wc(wc), m_bus(bus), m_type(type), m_time(std::move(time)),
        m_noRes(noRes) {
    for (int i = 0; i < RT_SA_MAX_COUNT; i++) {
      std::ostringstream stream;
      stream << std::setfill('0') << std::setw(4) << std::hex << std::uppercase
             << data[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      m_data.push_back(stream.str());
    }
  }

  U16BIT getRt() const { return m_rt; }
  U16BIT getSa() const { return m_sa; }
  U16BIT getRtRx() const { return m_rtRx; }
  U16BIT getSaRx() const { return m_saRx; }
  U16BIT wc() const { return m_wc; }
  char getBus() const { return m_bus; }
  std::string getType() const { return m_type; }
  std::string getTime() const { return m_time; }
  std::vector<std::string> getData() const { return m_data; }
  bool isResponded() const { return m_noRes; }

private:
  U16BIT m_rt;
  U16BIT m_sa;
  U16BIT m_rtRx;
  U16BIT m_saRx;
  U16BIT m_wc;
  char m_bus;
  const char *m_type;
  std::string m_time;
  std::vector<std::string> m_data;
  bool m_noRes;
};

#endif // MESSAGE_HPP