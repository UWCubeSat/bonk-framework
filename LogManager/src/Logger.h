// Copyright Eli Reed 2020, released under GPLv3

#include "LoggerBase.h"

namespace BONK {

class Logger : public LoggerBase {
  public:
    Logger();
    bool initialize(const char* path);
    bool log(const char* message);
};  // class Logger

}   // namespace BONK