#ifndef STATE_MANAGER_H_
#define STATE_MANAGER_H_

#include <stdint.h>     // for uint32_t, uint16_t, uint8_t

#include <EEPROM.h>     // for access to Arduino EEPROM
#include <SD.h>         // for access to SD card attached to Arduino


// Manages the state of an Arduino and attached devices.
template <typename S>
class StateManager
{
  public:
    // Constructs a StateManager. Uses file at filepath
    // to store overflow data. 
    StateManager(const char *filepath) : _write_size(sizeof(S)),
                                         _offset(sizeof(uint32_t) + sizeof(uint16_t)),
                                         _state_file_path(filepath) { }

    // Initializes StateManager with some initial state. Falls back
    // on fallback_state if EEPROM data are bad. Returns true if successful
    // false otherwise.
    bool initialize(const S& fallback_state);

    // returns the current state
    bool get_state(S& out) const {
        if (!_initialized) {
            return false;
        }
        out = _state;
        return true;
    }

    // sets the state and writes the update to the EEPROM. Returns true
    // if successful and false otherwise. Contents of state not guaranteed
    // if false.
    bool set_state(const S& state);

    // returns the number of successful writes to EEPROM
    bool get_write_count(uint16_t& out) const {
        if (!_initialized) {
            return false;
        }
        out = _write_count;
        return true;
    }

    // flushes contents of EEPROM to attached SD card. Returns
    // true if successul and false otherwise.
    bool flush_to_sd();

  private:
    // computes a crc32 of some state
    uint32_t crc32(S& state) const;

    // checks if EEPROM can store any more states
    bool filled() const;

    // appends a state to the EEPROM
    bool write_state(const S& state);

    // reads the last state from the EEPROM
    bool read_state(S& state) const;

    // number of written records
    uint16_t _write_count;

    // size of state record in bytes
    const uint16_t _write_size;

    // beginning of records in bytes
    const uint16_t _offset;

    // path to state file on SD Card
    const char* _state_file_path;

    // current state
    S _state;

    // is the state manager initialized?
    bool _initialized;
};  // StateManager class

template <typename S>
bool StateManager<S>::initialize(const S& fallback_state) {
    // read crc and write count
    uint32_t crc;
    uint16_t writes;
    uint32_t& ret_crc = EEPROM.get(0, crc);
    uint16_t& ret_writes = EEPROM.get(sizeof(uint32_t), writes);

    if (ret_crc != crc || ret_writes != writes) {
        return false;
    }

    if (writes == 0 || writes >= (EEPROM.length() - _offset) / _write_size) {
        // number of writes is weird, fallback on fallback_state
        _write_count = 0;
        _initialized = StateManager<S>::set_state(fallback_state);
    } else {
        // writes are reasonable, pull state and check crc
        _initialized = StateManager<S>::read_state(_state);
    }
    return _initialized;
}

template <typename S>
bool StateManager<S>::read_state(S& state) const {
    EEPROM.get(_offset + _write_size * (_write_count - 1), state);
    uint32_t crc;
    uint32_t& ret_crc = EEPROM.get(0, crc);
    return ret_crc == crc && StateManager::crc32(state) == crc;
}

template <typename S>
bool StateManager<S>::write_state(const S& state) {
    uint32_t crc = StateManager::crc32(const_cast<S&>(state));
    EEPROM.put(_offset + _write_size * (_write_count++), state);
    const uint16_t& ret_writes = EEPROM.put(sizeof(uint32_t), _write_count);
    const uint32_t& ret_crc = EEPROM.put(0, crc);
    return ret_writes == _write_count && ret_crc == crc;
}

template <typename S>
bool StateManager<S>::set_state(const S &state) {
    if (!_initialized) {
        return false;
    }
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
    if (!_initialized) {
        return false;
    }
    File sf = SD.open(_state_file_path, O_APPEND | O_WRITE);
    if (!sf) {
        return false;
    }
    for (uint16_t i = sizeof(uint32_t); i < _offset + _write_size * _write_count; i++) {
        if (sf.write(EEPROM[i]) == 0) {
            return false;
        }
    }
    sf.close();

    _write_count = 0;
    const uint16_t& ret_writes = EEPROM.put(_offset, _write_count);
    return ret_writes == _write_count;
}

// code adapted from https://www.arduino.cc/en/Tutorial/EEPROMCrc
template <typename S>
uint32_t StateManager<S>::crc32(S& state) const {
    constexpr uint32_t crc_table[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
    };

    uint32_t crc = ~0L;

    uint16_t size = sizeof(S);
    uint8_t* data = reinterpret_cast<uint8_t*>(&state);

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