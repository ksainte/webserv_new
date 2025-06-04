#include "../inc/Logger.hpp"

#include <ctime>
#include <vector>

const Logger::LogLevel Logger::CURRENT_LOG_LEVEL = DEBUG;
std::ofstream Logger::logFile;

void Logger::setLogFile(const std::string& filename) {
	if (logFile.is_open()) {
		logFile.close();
	}
	logFile.open(filename.c_str(), std::ios::app);
	if (!logFile.is_open()) {
		std::cerr << "Logger: Could not open log file " << filename << std::endl;
	}
}

void Logger::log(LogLevel level, const std::string& message, 
	std::ostream& output, const char* file, int line) 
{
	std::time_t now = std::time(NULL);
  std::tm* timeinfo = std::localtime(&now);

  std::string levelString;
  switch (level) {
    case DEBUG:    levelString = "DEBUG";    break;
    case INFO:     levelString = "INFO";     break;
    case WARNING:  levelString = "WARNING";  break;
    case ERROR:    levelString = "ERROR";    break;
    case CRITICAL: levelString = "CRITICAL"; break;
    default:       levelString = "UNKNOWN";  break;
  }

  std::vector<char> buffer(STRTIME_BUFFER);
  std::strftime(buffer.data(), buffer.size(), "%Y-%m-%d %H:%M:%S", timeinfo);

		output << "[" << buffer.data() << "] ";

		if (file && line) {
			output << "[" << file << ":" << line << "] ";
		}
		output << "[" << levelString << "] " << message << std::endl;

		if (logFile.is_open()) {

			std::vector<char> fileBuffer(STRTIME_BUFFER);
			std::strftime(fileBuffer.data(), fileBuffer.size(), "%Y-%m-%d %H:%M:%S", timeinfo);
			logFile << "[" << fileBuffer.data() << "] ";

			if (file && line) {
				logFile << "[" << file << ":" << line << "] ";
			}
			logFile << "[" << levelString << "] " << message << std::endl;
		}
}