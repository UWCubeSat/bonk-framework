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
		puts("hi!aloecrhuarloehuaoleruhaoruh");
		inCoast = true;
	}
};

TEST_CASE("Fires the on coast start event handler and updates the lastReading") {
	inCoast = false;
	CoastingEventHandler ceh;
	ceh.begin();
	Serial.FAKE_replaceBuffer("F,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1");
	// should read the whole thing
	ceh.tick();
	// make it think the packet is complete
	FAKE_millis = 50;
	ceh.tick();
	Bonk::ShipReading sh = ceh.getLastReading();
	REQUIRE(Bonk::FlightEvent::CoastStart == sh.event);
	REQUIRE(inCoast);
}

