// Copyright Eli Reed, 2020 released under GPLv3

#ifndef LOG_MANAGER_H_
#define LOG_MANAGER_H_

#include <SdFat.h>

namespace Bonk {

enum class LogType {
    DEBUG,
    WARNING,
    ERROR,
    NOTIFY,
};

class LogManager {
  public:
    LogManager() { }

    bool begin(const char* log_path, const char* data_path) {
	    if (log_path == nullptr) {
		    return false;
	    }
	    log_path_ = log_path;
	    log_file_.open(log_path_, O_WRITE | O_APPEND | O_CREAT);
    }

    size_t log(LogType level, const String& msg) {
	    return LogManager::log(level, msg.c_str());
    }
    size_t log(LogType level, const uint8_t* buf, size_t size) {
	    size_t bytes = print_tag(level);
	    switch (level) {
	    case LogType::DEBUG:
		    bytes += Serial.write(buf, size);
		    bytes += Serial.println();
		    break;
	    case LogType::WARNING:
	    case LogType::ERROR:
	    case LogType::NOTIFY:
		    bytes += log_file_.write(buf, size);
		    bytes += log_file_.write('\n');
		    break;
	    }
	    return bytes;
    }
    size_t log(LogType level, const char* msg) {
	    if (msg == nullptr) return 0;
	    ssize_t bytes = print_tag(level);
	    switch (level) {
	    case LogType::DEBUG:
		    bytes += Serial.println(msg);
		    break;
	    case LogType::WARNING:
	    case LogType::ERROR:
	    case LogType::NOTIFY:
		    bytes += log_file_.write(msg);
		    bytes += log_file_.write('\n');
		    break;
	    }
	    return bytes;
    }
  private:
    size_t print_tag(LogType level) {
	    String msg;
	    switch (level) {
	    case LogType::DEBUG:
		    return 0;
	    case LogType::WARNING:
		    msg = "[WARN] ";
		    break;
	    case LogType::ERROR:
		    msg = "[ERR] ";
		    break;
	    case LogType::NOTIFY:
		    msg = "[NOTIFY] ";
		    break;
	    }
	    msg += millis() + ": ";
	    return log_file_.print(msg.c_str());
    }
	
    const char* log_path_;
    FatFile log_file_;
};  // class LogManager

}   // BONK namespace

#endif  // LOG_MANAGER_H_
