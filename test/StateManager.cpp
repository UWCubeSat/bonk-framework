// Copyright (c) 2020 Mark Polyakov.
// Released under the GPLv3

#include "catch.hpp"

// shhhh, nobody has to know
#define private public
#include <StateManager.h>
#undef private

TEST_CASE("Starts in default state (from zapped)") {
  StateManager<unsigned char> sm("/blap");
  EEPROM.zap(0);
  unsigned char state;
  REQUIRE(!sm.get_state(state));
  REQUIRE(sm.initialize(123));
  REQUIRE(sm.get_state(state));
  REQUIRE(state == 123);
}

TEST_CASE("Starts in default state (from corrupted)") {
  StateManager<unsigned char> sm("/blap");
  EEPROM.zap(42);
  unsigned char state;
  REQUIRE(!sm.get_state(state));
  REQUIRE(sm.initialize(123));
  REQUIRE(sm.get_state(state));
  REQUIRE(state == 123);
}

TEST_CASE("Starts in default state (from corrupted. Part 2, electric boogaloo)") {
  StateManager<unsigned char> sm("/blap");
  EEPROM.zap(42);
  // manually set the write count
  EEPROM.put(sizeof(uint32_t), 14);
  unsigned char state;
  REQUIRE(!sm.get_state(state));
  REQUIRE(sm.initialize(123));
  REQUIRE(sm.get_state(state));
  REQUIRE(state == 123);
}

TEST_CASE("Returns correct state, even after many transitions") {
  StateManager<unsigned char> sm("/blap");
  EEPROM.zap(0);
  sm.initialize(0);
  for (int i = 0; i < 10239; i++) {
    unsigned char new_state = i * 3 % 256;
    unsigned char ret_state = 0;
    REQUIRE(sm.set_state(new_state));
    REQUIRE(sm.get_state(ret_state));
    REQUIRE(ret_state == new_state);
    StateManager<unsigned char> new_sm("/blap");
    REQUIRE(new_sm.initialize(0));
    REQUIRE(new_sm.get_state(ret_state));
    REQUIRE(ret_state == new_state);
  }
}

TEST_CASE("Fills up") {
  StateManager<unsigned char> sm("/blap");
  EEPROM.zap(0);
  sm.initialize(0);
  // 6 bytes for the header, and 1 byte from initialization.
  for (int i = 0; i < 255 - 6 - 1; i++) {
    sm.set_state(i);
    REQUIRE(!sm.filled());
  }
  sm.set_state(42);
  REQUIRE(sm.filled());
}

