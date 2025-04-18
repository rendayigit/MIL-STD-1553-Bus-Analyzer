#include "logger.hpp"

#include "common.hpp"

static std::shared_ptr<spdlog::logger> getFileLogger() {
  static std::shared_ptr<spdlog::logger> logger = [] {
    auto log = spdlog::daily_logger_mt("1553 Bus Log", getExecutableDirectory() + "1553.log", 0, 0);
    log->set_pattern("[%H:%M:%S.%f %z] [%n] [%l] [thread %t] %v");
    log->set_level(spdlog::level::trace);
    log->flush_on(spdlog::level::trace);
    return log;
  }();

  return logger;
}

static std::shared_ptr<spdlog::logger> getConsoleLogger() {
  static std::shared_ptr<spdlog::logger> logger = [] {
    auto log = spdlog::default_logger();
    log->set_pattern("[%H:%M:%S.%f %z] [%n] [%l] [thread %t] %v");
    log->set_level(spdlog::level::trace);
    log->flush_on(spdlog::level::trace);

    return log;
  }();

  return logger;
}

void Logger::info(const std::string &message) {
  getConsoleLogger()->info(message);
  getFileLogger()->info(message);
}

void Logger::error(const std::string &message) {
  getConsoleLogger()->error(message);
  getFileLogger()->error(message);
}

void Logger::warn(const std::string &message) {
  getConsoleLogger()->warn(message);
  getFileLogger()->warn(message);
}

void Logger::critical(const std::string &message) {
  getConsoleLogger()->critical(message);
  getFileLogger()->critical(message);
}

void Logger::debug(const std::string &message) {
  getConsoleLogger()->debug(message);
  getFileLogger()->debug(message);
}
