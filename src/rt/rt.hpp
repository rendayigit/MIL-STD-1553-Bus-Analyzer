#ifndef RT_HPP
#define RT_HPP

#include <stdemace.h>

#include "common.hpp"

class RT {
public:
  static RT &getInstance() {
    static RT instance;
    return instance;
  }

  S16BIT start(int devNum);
  S16BIT stop() const;

  S16BIT setRt(int rt, int sa, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> data);
  S16BIT activateRt(int rt, int sa);
  S16BIT deactivateRt(int rt, int sa);

private:
  RT();
  ~RT();

  U16BIT m_messageBuffer[RT_SA_MAX_COUNT]; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                                           // cppcoreguidelines-avoid-c-arrays)
  int m_devNum{};
};

#endif // RT_HPP