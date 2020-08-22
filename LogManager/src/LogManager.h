// Copyright Eli Reed, 2020 released under GPLv3

#ifndef LOG_MANAGER_H_
#define LOG_MANAGER_H_

#include "DataLogger.h"
#include "Logger.h"

namespace BONK {

enum LOG_TYPE {
    DEBUG,
    WARNING,
    ERROR,
    NOTIFY,
    DATA
};

class LogManager {
  public:
    LogManager();
    bool log(BONK::LOG_TYPE log_type, const char* message)
  private:
    DataLogger dl_;
    Logger logger_;
};  // class LogManager

}   // BONK namespace

#endif  // LOG_MANAGER_H_
