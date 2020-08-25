/**
 * Report the recordings of the temperature and power sensors to serial. Also
 * record the input from a certain pin from the containment unit I/O expander.
 * Comment out parts corresponding to sensors you aren't using.
 */

#define CONTAINMENT_SENSOR_PIN 0

#include <HardwareControl.h>

Bonk::Tmp411 thermometer;
Bonk::Pca9557 containmentPins;
Bonk::Main226 main226;
Bonk::Boost226 boost226;

void setup() {
	main226.begin();
	boost226.begin();
	HardwareController.begin(0.1); // 100mA current limit
	// Serial has to be initialized manually (it's not part of HardwareController)
	Serial.begin(115200);
	Serial.println("Starting...");
}

void loop() {
	Serial.println("");
	Serial.println("-----------");
	Serial.println("MAIN POWER SENSOR");
	Serial.println("Voltage: " + main226.readBusVoltage() + " V");
	Serial.println("Current: " + main226.readShuntCurrent() + " A");
	Serial.println("Power:   " + main226.readBusPower() + " W");

#ifdef BONK_BOOST
	Serial.println("BOOST POWER SENSOR");
	Serial.println("Voltage: " + boost226.readBusVoltage() + " V");
	Serial.println("Current: " + boost226.readShuntCurrent() + " A");
	Serial.println("Power:   " + boost226.readBusPower() + " W");
#endif

	Serial.println("TEMPERATURES");
	Serial.println(
		"Containment Board: " + thermometer.readLocalTemperature() + " °C";
	Serial.println(
		"Remote Sensor:     " + thermometer.readRemoteTemperature() + " °C");

	Serial.println("CONTAINMENT I/O PORT");
	Serial.println(
		"Pin " + CONTAINMENT_SENSOR_PIN + ": " +
		containmentPins.digitalRead(CONTAINMENT_SENSOR_PIN) ? "HIGH" : "LOW");

	Serial.println("-----------");
	delay(1000);
}
