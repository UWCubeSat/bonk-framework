// Copyright (c) 2020 Mark Polyakov
// Released under the GPLv3

#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#ifndef BONK_USB_SERIAL
#define BONK_USB_SERIAL Serial0
#endif

namespace Bonk {

  typedef enum FlightEvent {
#define BONK_FLIGHT_EVENT(blah, flightEvent) flightEvent,
#include "FlightEvents.h"
#undef BONK_FLIGHT_EVENT
  } FlightEvent;

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
		     _readingNormally(false) { }
		     // other fields can be uninitialized
		     
		     
    // Read bytes from serial, updating internal variables and calling event
    // handlers as necessary.
    void update();

    ShipReading getLastReading() const {
      return _lastReading;
    };

    // default event handlers -- all noop
#define BONK_FLIGHT_EVENT(blah, flightEvent) void on##flightEvent() const { }
#include "FlightEvents.h"
#undef BONK_FLIGHT_EVENT
  private:
    ShipReading _lastReading;
    ShipReading _partialReading;
    char _curField;
    char _buffer[16];
    unsigned char _buffer_n;
    bool _readingNormally;
  };  // class EventManager
}

#endif  // LOG_MANAGER_H_
