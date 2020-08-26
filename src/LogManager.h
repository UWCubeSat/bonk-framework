// Copyright Eli Reed, 2020 released under GPLv3

#ifndef LOG_MANAGER_H_
#define LOG_MANAGER_H_

#include <SdFat.h>

namespace BONK {

enum class LogType {
    DEBUG,
    WARNING,
    ERROR,
    NOTIFY,
};

class LogManager {
  public:
    LogManager() { }

    bool begin(const char* log_path, const char* data_path);

    size_t log(BONK::LogType level, const String& msg);
    size_t log(BONK::LogType level, const uint8_t* buf, size_t size);
    size_t log(BONK::LogType level, const char* msg);
  private:
    size_t print_tag(BONK::LogType level);

    const char* log_path_;
    SdFile log_file_;
};  // class LogManager

}   // BONK namespace

#endif  // LOG_MANAGER_H_
