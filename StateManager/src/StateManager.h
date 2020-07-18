#ifndef STATE_MANAGER_H_
#define STATE_MANAGER_H_

#include <stdint.h>   // for uint64_t, uint32_t

#include "Arduino.h"

template <typename S>
class StateManager
{
  public:
    StateManager(S &_default_state, const char * filepath);
    S& get_state() { return _state; }
    bool set_state(const S &state);
    bool flush_to_sd() const;

  private:
    unsigned long crc(void) const;
    bool filled() const;
    bool write_state(const S &state);
    bool read_state(S& state);

    uint64_t _write_size;
    uint32_t _write_count;
    const char* _state_file_path;
    S _default_state;
    S _state;
};  // StateManager class

#endif  // STATE_MANAGER_H_