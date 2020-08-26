/**
 * Turn every output on and off, one by one. An easy way to test all your 9557
 * connections.
 */

#include <BonkFramework.h>
#include <Wire.h>

Pca9557 containmentPins(BONK_CONTAINMENT9557_ADDRESS);

void setup() {
	Wire.begin();
	containmentPins.begin();
	for (char i = 0; i < 8; i++) {
		// will be low by default.
		containmentPins.pinMode(i, OUTPUT);
	}
}

void loop() {
	for (char i = 0; i < 8; i++) {
		containmentPins.digitalWrite(i, HIGH);
		delay(1000);
		containmentPins.digitalWrite(i, LOW);
	}
}
