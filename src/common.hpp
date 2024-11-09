#ifndef COMMON_HPP
#define COMMON_HPP

#include "logger/logger.hpp"
#include "stdemace.h"

#include <string>

constexpr int ACE_ERROR_BUFFER_SIZE = 80;
constexpr int HEX_BYTE = 16;
constexpr int RT_SA_MAX_COUNT = 32;

static Logger logger;

static std::string getStatus(S16BIT statusCode) {
  char buf[ACE_ERROR_BUFFER_SIZE]; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                                   // cppcoreguidelines-avoid-c-arrays)

  aceErrorStr(statusCode, buf, ACE_ERROR_BUFFER_SIZE);

  logger.log(statusCode == 0 ? LOG_INFO : LOG_ERROR, buf);

  return buf;
}

#endif // COMMON_HPP