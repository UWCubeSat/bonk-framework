/**
 * Report the recordings of the temperature and power sensors to serial. Also
 * record the input from a certain pin from the containment unit I/O expander.
 * Comment out parts corresponding to sensors you aren't using.
 */

#define CONTAINMENT_SENSOR_PIN 0
#define TMP411_ADDRESS 0b1001101 // depends on exact model of TMP411; check the datasheet.

#include <BonkFramework.h>

Bonk::Tmp411 thermometer(TMP411_ADDRESS);
Bonk::Pca9557 containmentPins(BONK_CONTAINMENT9557_ADDRESS);
Bonk::Main226 main226;
#ifdef BONK_BOOST
Bonk::Boost226 boost226;
#endif

void setup() {
	Wire.begin();
	Wire.setClock(400000); // the highest clock speed supported by the i/o expander.

	containmentPins.begin();
	thermometer.begin();

	// set this to the value of your current-sense resistors and desired current limit.
	// this is configured for 0.11 ohm for both shunts and a 100mA current limit
	main226.begin(0.11f, 0.1f);
#ifdef BONK_BOOST
	Bonk::enableBoostConverter(true);
	boost226.begin(0.11f);
#else
	Bonk::enableBoostConverter(false); // not really necessary; it's off by
																		 // default by a pulldown resistor.
#endif
	Serial.begin(115200);
	Serial.println("Starting...");
}

void loop() {
	Serial.println("");
	Serial.println("-----------");
	Serial.println("MAIN POWER SENSOR");
	Serial.print("Voltage: "); Serial.println(main226.readBusVoltage(), 4);
	Serial.print("Current: "); Serial.println(main226.readShuntCurrent(), 4);
	Serial.print("Power: ");   Serial.println(main226.readBusPower(), 4);

#ifdef BONK_BOOST
	Serial.println("BOOST POWER SENSOR");
	Serial.print("Voltage: "); Serial.println(boost226.readBusVoltage(), 4);
	Serial.print("Current: "); Serial.println(boost226.readShuntCurrent(), 4);
	Serial.print("Power: ");   Serial.println(boost226.readBusPower(), 4);
#endif

	Serial.println("TEMPERATURES");
	Serial.print("Containment Board: "); Serial.println(thermometer.readLocalTemperature() >> 8);
	Serial.print("Remote Sensor: ");     Serial.println(thermometer.readRemoteTemperature() >> 8);

	Serial.println("CONTAINMENT I/O PORT");
	Serial.print("Pin "); Serial.print(CONTAINMENT_SENSOR_PIN); Serial.print(": ");
	Serial.println(containmentPins.digitalRead(CONTAINMENT_SENSOR_PIN) ? "HIGH" : "LOW");

	Serial.println("-----------");
	delay(1000);
}
