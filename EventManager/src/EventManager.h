// Copyright Eli Reed, 2020 released under GPLv3
#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include <map>
#include <vector>

namespace BONK {

// Taken from NanoRacks Feather Frame PUG
enum FLIGHT_EVENT{
    NONE = 0x40,          // @
    ESCAPE_ENABLED,       // A
    ESCAPE_COMMANDED,     // B
    LIFTOFF,              // C
    MAIN_ENGINE_CUTOFF,   // D
    SEPARATION_COMMANDED, // E
    COAST_START,          // F
    APOGEE,               // G
    COAST_END,            // H
    DROGUE_CHUTES,        // I
    MAIN_CHUTES,          // J
    TOUCHDOWN,            // K
    SAFING,               // L
    MISSION_END,          // M
};

class EventManager {
  public:
    EventManager();
    bool register_event(const BONK::FLIGHT_EVENT fe, bool (*function)());
  private:
    std::map<BONK::FLIGHT_EVENT, std::vector<bool (*)()>> cb_func_map_;
};  // class LogManager

} // namespace BONK

#endif  // LOG_MANAGER_H_
