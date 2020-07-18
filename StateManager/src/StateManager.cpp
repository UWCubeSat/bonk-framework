#include <EEPROM.h>
#include <SD.h>

#include "./StateManager.h"

static constexpr uint8_t kOffset = 10;

static constexpr uint16_t kMagicNumber = 0x451b;

static constexpr uint8_t kDataStart = 14;

template <typename S>
StateManager<S>::StateManager(S &default_state, const char *filepath) {
    _default_state = default_state;
    _state_file_path = filepath;
    _write_size = sizeof(S);

    uint64_t crc;
    EEPROM.get(0, crc);
    uint64_t real_crc = StateManager::crc();

    uint16_t magic_num;
    EEPROM.get(sizeof(uint16_t), magic_num);

    if (crc == real_crc && magic_num == kMagicNumber) {
        // the data in the EEPROM is good
        // load the last state we saved
        EEPROM.get(kOffset, _write_count);
        StateManager<S>::read_state(_state);
    } else {
        // the data in the EEPROM are bad
        // fall back to the default state
        StateManager<S>::set_state(_default_state);
        EEPROM.put(sizeof(uint64_t), kMagicNumber);
        EEPROM.put(0, StateManager::crc());
    }
}

template <typename S>
bool StateManager<S>::read_state(S &state) {
    S& ret = EEPROM.get(kDataStart + _write_size * (_write_count - 1), state);
    if (ret != nullptr) {
        return true;
    }
    return false;
}

template <typename S>
bool StateManager<S>::write_state(const S &state) {
    S& ret = EEPROM.put(kDataStart + _write_size * _write_count, state);
    if (ret != nullptr) {
        _write_count++;
        EEPROM.put(kOffset, _write_count);
        EEPROM.put(0, StateManager<S>::crc());
        return true;
    }
    return false;
}

template <typename S>
bool StateManager<S>::set_state(const S &state) {
    if (state == nullptr) {
        return false;
    }

    if (StateManager<S>::filled()) {
        if (!StateManager<S>::flush_to_sd(_state_file_path)) {
            return false;
        }
    }

    bool ret = StateManager<S>::write_state(state);

    if (StateManager<S>::write_state(state)) {
        _state = state;
        return true;
    }
    return false;
}

template <typename S>
bool StateManager<S>::flush_to_sd() const {
    File sf = SD.open(_state_file_path, O_APPEND | O_WRITE);
    char buf[EEPROM.length() - kOffset];
    for (uint32_t i = kOffset; i < EEPROM.length(); i++) {
        buf[i] = EEPROM[i];
    }
    sf.write(buf, EEPROM.length() - kOffset);
    sf.close();

    _write_count = 0;
    EEPROM.put(kOffset, _write_count);
    EEPROM.put(0, StateManager<S>::crc());
    return true;
}

// code taken from https://www.arduino.cc/en/Tutorial/EEPROMCrc
template <typename S>
unsigned long StateManager<S>::crc(void) const {

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

template <typename S>
bool StateManager<S>::filled() const {
    return (_write_size * (_write_count + 1) + kDataStart) > EEPROM.length();
}
