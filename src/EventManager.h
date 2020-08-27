// Copyright (c) 2020 Mark Polyakov
// Released under the GPLv3

#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#ifndef BONK_USB_SERIAL
#define BONK_USB_SERIAL Serial
#endif

namespace Bonk {

  typedef enum FlightEvent {
#define BONK_FLIGHT_EVENT(blah, flightEvent) flightEvent,
#include "FlightEvents.h"
#undef BONK_FLIGHT_EVENT
  } FlightEvent;

  const char NUM_FIELDS = 21;
  const char NUM_LONG_FIELDS = 16;

  // AVR, as an 8-bit architecture, aligns fields to 1 byte anyway. The
  // attribute just makes absolutely sure.
  typedef struct __attribute__((packed)) ShipReading {
    long elapsed; // milliseconds, unsigned
    long altitude; // micrometers, unsigned
    long gpsAltitude; // micrometers, unsigned
    long vx; // micrometers/second
    long vy; // micrometers/second
    long vz; // micrometers/second
    long aTotal; // unsigned, micrometers/second^2
    long ax; // micrometers/second^2
    long ay; // micrometers/second^2
    long az; // micrometers/second^2
    long phi; // microradians
    long theta; // microradians
    long psi; // microradians
    long angx; // microradians
    long angy; // microradians
    long angz; // microradians
    bool launchImminent;
    bool drogueChuteImminent;
    bool landingImminent;
    bool chuteFaultWarning;
    FlightEvent event;
  } ShipReading;

  // should be subclassed, adding event handlers.
  class EventManager {
  public:
    EventManager() : _lastReading({ 0 }), // default flight event is NoneReached
		     _curField(0),
		     _readingNormally(false) { };
		     // other fields can be uninitialized

    void begin() {
      _lastDataMillis = millis() % 256;
    }

    // call this every loop(). The more often you call it, the better! If you
    // don't call it at least once 75ms or so, things will get nasty
    void tick() {
      uint8_t curMillis = millis() % 256;
      uint8_t millisSinceLastData = curMillis - _lastDataMillis;
      char incomingChar = BONK_USB_SERIAL.read();

      if (incomingChar > -1) {
	if (millisSinceLastData > 75) {
	  // TODO: log that we're running behind
	  // for now, the current reading is marked as invalid and discarded.
	  _readingNormally = false;
	  _finishReading();
	  return;
	}

	do {
	  _processCharacter(incomingChar);
	} while ((incomingChar = BONK_USB_SERIAL.read()) > -1);

      } else { // incomingChar == -1, ie, no data available
	if (millisSinceLastData > 2) {
	  _finishReading();
	  _runEvents();
	}
      }
    }

    ShipReading getLastReading() const {
      return _lastReading;
    };

  protected:
    // default event handlers -- all noop
#define BONK_FLIGHT_EVENT(blah, flightEvent) void on##flightEvent() const { };
#include "FlightEvents.h"
#undef BONK_FLIGHT_EVENT

  private:
    static const uint8_t NUM_BUFFER_CHARS = 16;
    ShipReading _lastReading;
    ShipReading _partialReading;
    uint8_t _lastDataMillis;          // millis() % 256
    uint8_t _curField;                // integer indicating which field we are currently reading
    char _buffer[NUM_BUFFER_CHARS];   // the currently (partially) read field
    uint8_t _buffer_n;                // index into _buffer, of the first empty spot
    bool _readingNormally;            // false if any fatal errors been detected in the current reading
    // TODO: store the total number of packets that failed to read properly?

    // mark the reading as failed TODO: log an error
    void _failReading() {
      _readingNormally = false;
    }

    // attempt to parse _buffer and insert it into partialReading
    void _finishField() {
      if (_buffer_n < NUM_BUFFER_CHARS) { // if this test fails, _readingNormally already set to
                                          // false by _processCharacter
        _buffer[_buffer_n] = '\0';

        if (_curField == 0) { // special case for flight event
          // should only be one character, and it should be one of the known
          // flight event types.
          if (_buffer_n == 1) {
            switch (_buffer[0]) {
#define BONK_FLIGHT_EVENT(eventChar, flightEvent) case eventChar:       \
              _partialReading.event = flightEvent;                      \
              break;
#include "FlightEvents.h"
#undef BONK_FLIGHT_EVENT
            default:
              _failReading();
            }
          } else {
            _failReading();
          }

        } else if (_curField < 1 + NUM_LONG_FIELDS) { // one of the longs
          ((long *)(&_partialReading))[_curField - 1] = atol(_buffer);

        } else if (_curField < NUM_FIELDS) { // one of the bools
          bool val = false;
          // enforce it being either 0 or 1
          if (_buffer[0] == '1') {
            val = true;
          } else if (_buffer[0] != '0') {
            _failReading();
          }
          // we could skip this if reading normally check above failed
          ((bool *)(&_partialReading.launchImminent))[_curField - NUM_LONG_FIELDS] = val;

        } else { // too many fields
          _failReading();
        }
      }

      _buffer_n = 0;
    };

    // check if partialReading is legit, and if so move it into lastReading
    void _finishReading() {
      _finishField();
      if (
	_readingNormally &&
	_curField == NUM_FIELDS + 1) {

	_lastReading = _partialReading;
      }
      // unconditionally reset the state machine
      _curField = 0;
      _readingNormally = true;
      // buffer_n already set by finishField
    };

    // run the event corresponding to lastReading
    void _runEvents() {
      switch (_lastReading.event) {
#define BONK_FLIGHT_EVENT(eventChar, flightEvent) case eventChar: \
        on##flightEvent(); \
        break;
#include "FlightEvents.h"
#undef BONK_FLIGHT_EVENT
      }
    }

    void _processCharacter(char incoming) {
      if (_buffer_n == NUM_BUFFER_CHARS) {
        _failReading();
      } else if (incoming != '.' || _curField == 1) { // don't store dots, unless part of elapsed time
        _buffer[_buffer_n++] = incoming;
      }
    };

  };  // class EventManager
}

#endif  // LOG_MANAGER_H_
