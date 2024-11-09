#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "common.hpp"
#include <iomanip>
#include <string>
#include <utility>
#include <vector>

class Message {
public:
  explicit Message(U16BIT rt, U16BIT sa, U16BIT rtRx, U16BIT saRx, U16BIT wc, char bus, const char *type, std::string time,
                   U32BIT number, const U16BIT *data)
      : m_rt(rt), m_sa(sa), m_rtRx(rtRx), m_saRx(saRx), m_wc(wc), m_bus(bus), m_type(type), m_time(std::move(time)),
        m_number(number) {
    for (int i = 0; i < m_wc; i++) {
      std::ostringstream stream;
      stream << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << data[i];
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
  U64BIT getNumber() const { return m_number; }
  std::vector<std::string> getData() const { return m_data; }

private:
  U16BIT m_rt;
  U16BIT m_sa;
  U16BIT m_rtRx;
  U16BIT m_saRx;
  U16BIT m_wc;
  char m_bus;
  const char *m_type;
  std::string m_time;
  U64BIT m_number;
  std::vector<std::string> m_data;
};

#endif // MESSAGE_HPP