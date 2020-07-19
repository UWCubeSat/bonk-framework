#ifndef STATE_MANAGER_H_
#define STATE_MANAGER_H_

#include <stdint.h>   // for uint64_t, uint32_t

#include <EEPROM.h>
#include <SD.h>

template <typename S>
class StateManager
{
  public:
    StateManager(const S& default_state, const char *filepath);
    S get_state() const { return _state; }
    bool set_state(const S& state);
    uint16_t get_write_count() const { return _write_count; }
    bool flush_to_sd();

  private:
    uint32_t crc32(const S& state) const;
    bool filled() const;
    bool write_state(const S& state);
    bool read_state(S& state) const;

    uint16_t _write_count;
    const uint16_t _write_size;
    const uint16_t _offset;
    const char* _state_file_path;
    const S _default_state;
    S _state;
};  // StateManager class

template <typename S>
StateManager<S>::StateManager(const S& default_state, const char *filepath) :
                              _write_size(sizeof(S)),
                              _offset(sizeof(uint32_t) + sizeof(uint16_t)),
                              _state_file_path(filepath),
                              _default_state(default_state) {

    // uint32_t crc;
    // EEPROM.get(0, crc);
    // uint32_t real_crc = StateManager::crc();

    // uint16_t magic_num;
    // EEPROM.get(sizeof(uint32_t), magic_num);

    // if (crc == real_crc && magic_num == MAGIC_NUMBER) {
    //     // the data in the EEPROM is good
    //     // load the last state we saved
    //     EEPROM.get(_offset, _write_count);
    //     StateManager<S>::read_state(_state);
    // } else {
    //     // the data in the EEPROM are bad
    //     // fall back to the default state
    //     StateManager<S>::set_state(_default_state);
    //     EEPROM.put(sizeof(uint32_t), _magic_number);
    //     EEPROM.put(0, StateManager::crc());
    // }
}

template <typename S>
bool StateManager<S>::read_state(S& state) const {
    S& ret_state = EEPROM.get(_offset + _write_size * (_write_count - 1), state);
    uint32_t crc;
    uint32_t& ret_crc = EEPROM.get(0, crc);
    return ret_crc == crc && StateManager::crc32(state) == crc && ret_state == state;
}

template <typename S>
bool StateManager<S>::write_state(const S& state) {
    uint32_t crc = StateManager::crc32(state);
    const S& ret_state = EEPROM.put(_offset + _write_size * (_write_count++), state);
    const uint16_t& ret_writes = EEPROM.put(sizeof(uint32_t), _write_count);
    const uint32_t& ret_crc = EEPROM.put(0, crc);
    return ret_state == state && ret_writes == _write_count && ret_crc == crc;
}

template <typename S>
bool StateManager<S>::set_state(const S &state) {
    if (StateManager::filled()) {
        if (!StateManager::flush_to_sd()) {
            return false;
        }
    }
    
    if (StateManager::write_state(state)) {
        _state = state;
        return true;
    }
    return false;
}

template <typename S>
bool StateManager<S>::flush_to_sd() {
    File sf = SD.open(_state_file_path, O_APPEND | O_WRITE);
    for (uint16_t i = sizeof(uint16_t); i < EEPROM.length(); i++) {
        sf.write(EEPROM[i]);
    }
    sf.close();

    _write_count = 0;
    const uint16_t& ret_writes = EEPROM.put(_offset, _write_count);
    return ret_writes == _write_count;
}

// code adapted from https://www.arduino.cc/en/Tutorial/EEPROMCrc
template <typename S>
uint32_t StateManager<S>::crc32(const S& state) const {
    constexpr uint32_t crc_table[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
    };

    uint32_t crc = ~0L;

    uint16_t size = sizeof(S);
    uint8_t* data = &state;

    for (uint16_t index = 0 ; index < size; ++index) {
        crc = crc_table[(crc ^ (*(data + index))) & 0x0f] ^ (crc >> 4);
        crc = crc_table[(crc ^ (*(data + index)) >> 4) & 0x0f] ^ (crc >> 4);
        crc = ~crc;
    }
    return crc;
}

template <typename S>
bool StateManager<S>::filled() const {
    return (_write_size * (_write_count + 1) + _offset) >= EEPROM.length();
}

#endif  // STATE_MANAGER_H_