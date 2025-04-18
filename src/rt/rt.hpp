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
  S16BIT activateRt(int rt);
  S16BIT deactivateRt(int rt);

private:
  RT();
  ~RT();

  int m_devNum{};
};

#endif // RT_HPP