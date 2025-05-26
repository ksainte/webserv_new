#pragma once
#include <fstream>
#include <iostream>
#include <sstream>

class Logger
{
public:
  enum LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

  static const LogLevel CURRENT_LOG_LEVEL;
  static const int STRTIME_BUFFER = 20;
  static std::ofstream logFile;

  static void setLogFile(const std::string& filename);

  static void log(LogLevel level, const std::string& message,
                  std::ostream& output, const char* file = NULL, int line = 0);

  // --- New: Helper class for building log messages using streams ---
  class LogBuilder
  {
  private:
    Logger::LogLevel level;
    const char* file;
    int line;
    std::stringstream ss;
    bool enabled;

  public:
    LogBuilder(Logger::LogLevel lvl, const char* f, int l)
      : level(lvl), file(f), line(l), enabled(lvl >= Logger::CURRENT_LOG_LEVEL)
    {
    }

    ~LogBuilder()
    {
      if (enabled)
      {
        std::ostream& output_stream = (level == Logger::ERROR || level == Logger::CRITICAL) ? std::cerr : std::clog;
        Logger::log(level, ss.str(), output_stream, file, line);
      }
    }

    template <typename T>
    LogBuilder& operator<<(const T& value)
    {
      if (enabled)
      {
        ss << value;
      }
      return *this;
    }
  };
};

#define LOG_BUILDER(level) \
    Logger::LogBuilder(level, __FILE__, __LINE__)

#define LOG_DEBUG LOG_BUILDER(Logger::DEBUG)
#define LOG_INFO LOG_BUILDER(Logger::INFO)
#define LOG_WARNING LOG_BUILDER(Logger::WARNING)
#define LOG_ERROR LOG_BUILDER(Logger::ERROR)
#define LOG_CRITICAL LOG_BUILDER(Logger::CRITICAL)
