/**
 * Report the recordings of the temperature and power sensors to serial. Also
 * record the input from a certain pin from the containment unit I/O expander.
 */

#define CONTAINMENT_SENSOR_PIN 0

#define BONK_BOOST // remove if you don't use the boost converter
#include <HardwareControl.h>

void setup() {
	HardwareController.begin(0.1); // 100mA current limit
	// Serial has to be initialized manually (it's not part of HardwareController)
	Serial.begin(115200);
	Serial.println("Starting...");
}

void loop() {
	Serial.println("");
	Serial.println("-----------");
	Serial.println("MAIN POWER SENSOR");
	Serial.println("Voltage: " + HardwareController.readMainVoltage());
	Serial.println("Current: " + HardwareController.readMainCurrent());
	Serial.println("Power:   " + HardwareController.readMainPower());

#ifdef BONK_BOOST
	Serial.println("BOOST POWER SENSOR");
	Serial.println("Voltage: " + HardwareController.readBoostVoltage() + " V");
	Serial.println("Current: " + HardwareController.readBoostCurrent() + " A");
	Serial.println("Power:   " + HardwareController.readBoostPower() + " W");
#endif

	Serial.println("TEMPERATURES");
	Serial.println(
		"Containment Board: " + HardwareController.readContainmentBoardTemperature()) + " °C";
	Serial.println(
		"Remote Sensor:     " + HardwareController.readRemoteTemperature() + " °C");

	Serial.println("CONTAINMENT I/O PORT");
	Serial.println(
		"Pin " + CONTAINMENT_SENSOR_PIN + ": " +
		HardwareController.containmentDigitalRead(CONTAINMENT_SENSOR_PIN) ? "HIGH" : "LOW");

	Serial.println("-----------");
	delay(1000);
}
