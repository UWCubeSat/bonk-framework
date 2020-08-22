// Copyright Eli Reed 2020, released under GPLv3

#ifndef LOGGER_BASE_H_

#include <SdFat.h>

namespace BONK {

// Abstract class 
class LoggerBase {
  public:
    virtual bool initialize(const char* path) = 0;
    virtual bool log(const char* message) = 0;
  private:
    const char* file_path_;
    SdFile file_;
};      // class LogBase

}   // namespace BONK

#endif  // LOG_BASE_H_