// Copyright Eli Reed, 2020 released under GPLv3

#ifndef LOG_MANAGER_H_
#define LOG_MANAGER_H_

#include <SdFat.h>

namespace BONK {

typedef enum LOG_TYPE {
    DEBUG,
    WARNING,
    ERROR,
    NOTIFY,
    DATA,
    DATA_DEBUG,
} LogType;

class LogManager {
  public:
    LogManager() : last_write_successful_(false) { }

    bool begin(const char* log_path, const char* data_path);

  private:
    const char* data_path_;
    SdFile data_file_;

    const char* log_path_;
    SdFile log_file_;
};  // class LogManager

}   // BONK namespace

#endif  // LOG_MANAGER_H_
