#include "logger.hpp"
#include "fileOperations/fileOperations.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

// Map to convert LogLevel enum values to string representations
const std::map<LogLevel, std::string> logLevelMap = {
    {LogLevel::LOG_ERROR, "ERROR"}, {LogLevel::LOG_INFO, "INFO"}, {LogLevel::LOG_WARNING, "WARNING"}};

Logger::Logger(std::string logFilePath, int logBufferLimit)
    : m_logFilePath(std::move(logFilePath)), m_logBufferLimit(logBufferLimit) {
  // Get current date as string
  std::string localDate = boost::gregorian::to_simple_string(boost::posix_time::second_clock::local_time().date());

  // Set log file name and directory
  if (m_logFilePath.empty()) {
    // Default log file name and directory if no log file path is provided
    m_logFileName = localDate + ".log";
    m_logDirectory = FileOperations::getInstance().getExecutableDirectory();
  } else {
    // Set log file name and directory based on the provided log file path
    size_t found = m_logFilePath.find_last_of('/');

    if (found != std::string::npos) {
      m_logFileName = m_logFilePath.substr(found + 1);
      m_logDirectory = m_logFilePath.substr(0, found + 1);
    } else {
      m_logFileName = m_logFilePath;
      m_logDirectory = FileOperations::getInstance().getExecutableDirectory();
    }
  }
}

Logger::~Logger() {
  if (m_logFile.is_open()) {
    m_logFile << m_logBuffer; // Write buffer to file writer
    m_logFile.flush();        // Flush the file writer
  }
}

void Logger::log(LogLevel level, const std::string &message) {
  std::lock_guard<std::mutex> lock(m_mutex); // Ensure thread-safe access

  // Get current time and date as string
  std::string localTime = boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time());

  // Append the time, log level and log message to the buffer
  m_logBuffer += localTime + " " + logLevelMap.at(level) + " " + message + '\n';

  // Create the log directory if it doesn't exist
  if (not boost::filesystem::exists(m_logDirectory)) {
    boost::filesystem::create_directory(m_logDirectory);
  }

  // Open the log file if it's not already open
  if (not m_logFile.is_open()) {
    m_logFile.open(m_logDirectory + m_logFileName, std::ios::app);
  }

  // Write the buffer to the log file if it exceeds the limit
  if (m_logBuffer.size() >= m_logBufferLimit) {
    flush();
  }
}

void Logger::flush() {
  if (m_logFile.is_open()) {
    m_logFile << m_logBuffer; // Write buffer to file writer
    m_logFile.flush();        // Flush the file writer
    m_logBuffer = "";         // Clear the buffer after writing
  } else {
    // Log an error if the file can't be opened
    std::cerr << "Error: Failed to open the log file." << '\n';
    return;
  }

  // Close the file writer
  m_logFile.close();
}
