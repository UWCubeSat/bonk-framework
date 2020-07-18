#include <EEPROM.h>
#include <SD.h>

#include "./StateManager.h"

static constexpr uint8_t kOffset = 5;

template <class S>
StateManager::StateManager<S>() : {
    StateManager::_write_size = sizeof(S);
    EEPROM.get(0, StateManager::_write_count);
    EEPROM.get(sizeof(uint32_t), StateManager::_write_size);
    unsigned long real_crc = StateManager::crc();
    unsigned long crc;
    EEPROM.get(sizeof(unsigned long) + sizeof(uint32_t), crc);
    if (real_crc != crc) {
        // something bad has happened; what to do?
    }
}

template<class S>
bool StateManager::read_state<S>(S& state) {
    S& ret = EEPROM.get(kOffset + StateManager::_write_size * (StateManager::_write_count - 1), state)
    if (ret != nullptr) {
        return true;
    }
    return false;
}

template <class S>
bool StateManager::write_state<S>(const S& state) {
    S& ret = EEPROM.put(kOffset + StateManager::_write_size * StateManager::_write_count, state);
    if (ret != nullptr) {
        StateManager::_write_count++;
        EEPROM.put(0, StateManager::_write_count);
        return true;
    }
    return false;    
}

template <class S>
bool StateManager::set_state<S>(const S& state) {
    if (state == nullptr) {
        return false;
    }

    if (StateManager::filled()) {
        StateManager::flush_to_sd();
    }

    bool ret = StateManager::write_state(state);

    if (StateManager::write_state(state)) {
        _last_state = _state;
        _state = state;
        return true;
    }
    return false;
}

bool StateManager::flush_to_sd() const {
    
}

// code taken from https://www.arduino.cc/en/Tutorial/EEPROMCrc
unsigned long StateManager::crc(void) {

  constexpr uint64_t crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  uint64_t crc = ~0L;

  for (int index = 0; index < EEPROM.length(); ++index) {
    crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}

bool StateManager::filled() const {
    return (_write_size * (_write_count + 1) + kOffset) > 
}
