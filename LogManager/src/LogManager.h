#ifndef LOG_MANAGER_H_
#define LOG_MANAGER_H_

#include <SD.h>         // for access to SD card attached to Arduino

namespace BONK {
  enum LOG_LEVEL{DEBUG, WARNING, ERROR, NOTIFY};
  class LogManager {
    public:
      LogManager(const char* log_path);
      get_log_level() { return log_level_;}
      set_log_level(const BONK::LOG_LEVEL level) {log_level_ = level}
      log(const char* message);      
    private:
      LOG_LEVEL log_level_;
      const char* log_path;

  };  // class LogManager
  extern LogManager Logger;
}   // BONK namespace

#endif  // LOG_MANAGER_H_
