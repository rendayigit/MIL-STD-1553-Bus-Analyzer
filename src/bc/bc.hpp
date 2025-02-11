#ifndef BC_HPP
#define BC_HPP

#include <array>
#include <stdemace.h>
#include <string>

#include "common.hpp"

class BC {
public:
  static BC &getInstance() {
    static BC instance;
    return instance;
  }

  S16BIT start(int devNum);
  S16BIT stop() const;
  S16BIT bcToRt(int rt, int sa, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> data);
  S16BIT rtToBc(int rt, int sa, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> *data);
  S16BIT rtToRt(int rtTx, int saTx, int rtRx, int saRx, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> *data);

private:
  BC();
  ~BC();

  U16BIT m_messageBuffer[RT_SA_MAX_COUNT]; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                                           // cppcoreguidelines-avoid-c-arrays)
  int m_devNum{};
};

#endif // BC_HPP