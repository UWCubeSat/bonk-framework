#ifndef STATE_MANAGER_H_
#define STATE_MANAGER_H_

#include <stdint.h>   // for uint64_t, uint32_t

#include "Arduino.h"

template <class S>
class StateManager
{
  public:
    StateManager();
    bool read_state(S& state);
    S& get_state() { return _state };
    bool write_state(const S& state);
    bool set_state(const S& state);
    bool flush_to_sd() const;

  private:
    unsigned long crc(void) const;
    bool filled() const;

    uint64_t _write_size;
    S _state;
    S _last_state;
};  // StateManager class

#endif  // STATE_MANAGER_H_