// Copyright Eli Reed 2020, released under GPLv3

#include "LoggerBase.h"

namespace BONK {

class DataLogger : public LoggerBase {
  public:
    DataLogger();
    bool initialize(const char* path);
    bool log(const char* message);
};    // class DataLogger

}   // namespace BONK