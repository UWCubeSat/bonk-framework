// Copyright (c) 2020 Mark Polyakov
// Released under the GPLv3

#include "catch.hpp"

#include "otherMocks.h"
#include "Serial.h"

#include <LogManager.h>
#include <EventHandler.h>

bool inCoast;

class CoastingEventHandler: public Bonk::EventHandler {
protected:
	void onCoastStart() const override {
		inCoast = true;
	}
};

Bonk::ShipReading give_buffer(const char* blah) {
	inCoast = false;
	CoastingEventHandler ceh;
	ceh.begin();
	Serial.FAKE_replaceBuffer(blah);
	// should read the whole thing
	ceh.tick();
	// make it think the packet is complete
	FAKE_millis = 100;
	ceh.tick();
	return ceh.getLastReading();
}

TEST_CASE("Fires the on coast start event handler and updates the lastReading") {
	Bonk::ShipReading sh = give_buffer("F,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1");
	REQUIRE(Bonk::FlightEvent::CoastStart == sh.event);
	REQUIRE(inCoast);
}

TEST_CASE("Corrupted: too many fields") {
	give_buffer("F,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5");
	REQUIRE(!inCoast);
}

TEST_CASE("Corrupted: Too few fields") {
	give_buffer("F,1,1,1,1,1,1,1");
	REQUIRE(!inCoast);
}

TEST_CASE("Corrupted: Invalid event") {
	give_buffer("*,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1");
	REQUIRE(!inCoast);
}

TEST_CASE("Correct values for data") {
	Bonk::ShipReading sh = give_buffer("F,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1");
//	Bonk::ShipReading sh = give_buffer("A,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1");
	REQUIRE(sh.event == Bonk::FlightEvent::EscapeEnabled);
	REQUIRE(sh.vx == 1234567L);
}
