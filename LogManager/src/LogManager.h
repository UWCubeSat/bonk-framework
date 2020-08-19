#ifndef LOG_MANAGER_H_
#define LOG_MANAGER_H_

#include <SD.h>         // for access to SD card attached to Arduino

namespace BONK {
  enum LOG_LEVEL{DEBUG, WARNING, ERROR, NOTIFY};
  class LogManager {
    public:
      LogManager(const char* log_path);     
    private:
      LOG_LEVEL log_level_;
      const char* log_path;

  };  // class LogManager
}   // BONK namespace

#endif  // LOG_MANAGER_H_
