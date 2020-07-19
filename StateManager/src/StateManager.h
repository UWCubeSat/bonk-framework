#ifndef STATE_MANAGER_H_
#define STATE_MANAGER_H_

#include <stdint.h>   // for uint64_t, uint32_t

#include <EEPROM.h>
#include <SD.h>

template <typename S>
class StateManager
{
  public:
    StateManager(S &default_state, const char *filepath);
    S get_state() const { return _state; }
    bool set_state(const S &state);
    bool flush_to_sd();

  private:
    unsigned long crc(void) const;
    bool filled() const;
    bool write_state(const S &state);
    bool read_state(S& state);

    uint64_t _write_size;
    uint32_t _write_count;
    const char* _state_file_path;
    const uint8_t _offset;
    const uint16_t _magic_number;
    const uint8_t _data_start;
    S _default_state;
    S _state;
};  // StateManager class

template <typename S>
StateManager<S>::StateManager(S &default_state, const char *filepath) : 
                              _offset(sizeof(uint32_t) + sizeof(uint16_t)),
                              _magic_number(0x451b),
                              _data_start(_offset + sizeof(uint32_t)) {
    _default_state = default_state;
    _state_file_path = filepath;
    _write_size = sizeof(S);

    uint32_t crc;
    EEPROM.get(0, crc);
    uint32_t real_crc = StateManager::crc();

    uint16_t magic_num;
    EEPROM.get(sizeof(uint32_t), magic_num);

    if (crc == real_crc && magic_num == _magic_number) {
        // the data in the EEPROM is good
        // load the last state we saved
        EEPROM.get(_offset, _write_count);
        StateManager<S>::read_state(_state);
    } else {
        // the data in the EEPROM are bad
        // fall back to the default state
        StateManager<S>::set_state(_default_state);
        EEPROM.put(sizeof(uint32_t), _magic_number);
        EEPROM.put(0, StateManager::crc());
    }
}

template <typename S>
bool StateManager<S>::read_state(S &state) {
    S& ret = EEPROM.get(_data_start + _write_size * (_write_count - 1), state);
    return true;
}

template <typename S>
bool StateManager<S>::write_state(const S &state) {
    const S& ret = EEPROM.put(_data_start + _write_size * _write_count, state);
    _write_count++;
    EEPROM.put(_offset, _write_count);
    EEPROM.put(0, StateManager<S>::crc());
    return true;
}

template <typename S>
bool StateManager<S>::set_state(const S &state) {
    if (StateManager::filled()) {
        if (!StateManager::flush_to_sd()) {
            return false;
        }
    }

    if (StateManager<S>::write_state(state)) {
        _state = state;
        return true;
    }
    return false;
}

template <typename S>
bool StateManager<S>::flush_to_sd() {
    File sf = SD.open(_state_file_path, O_APPEND | O_WRITE);
    char buf[EEPROM.length() - _offset];
    for (uint32_t i = _offset; i < EEPROM.length(); i++) {
        buf[i - _offset] = EEPROM[i];
    }
    sf.write(buf, EEPROM.length() - _offset);
    sf.close();

    _write_count = 0;
    EEPROM.put(_offset, _write_count);
    EEPROM.put(0, StateManager::crc());
    return true;
}

// code taken from https://www.arduino.cc/en/Tutorial/EEPROMCrc
template <typename S>
uint32_t StateManager<S>::crc(void) const {

    constexpr uint32_t crc_table[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
    };

    uint32_t crc = ~0L;

    // we only want to fold bytes that AREN'T are the CRC segment/magic number
    for (int index = _offset; index < EEPROM.length(); ++index) {
        crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);
        crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
        crc = ~crc;
    }
    return crc;
}

template <typename S>
bool StateManager<S>::filled() const {
    return (_write_size * (_write_count + 1) + _data_start) > EEPROM.length();
}

#endif  // STATE_MANAGER_H_