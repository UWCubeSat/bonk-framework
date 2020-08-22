#include <Wire.h>

#include "HardwareControl.h"

HardwareControl::begin(const float currentLimit) {
  main226.begin(BONK_MAIN226_ADDRESS);
  main226.configure(INA226_AVERAGES_4,
		    INA226_BUS_CONV_TIME_140US,
		    INA226_SHUNT_CONV_TIME_140US,
		    INA226_MODE_SHUNT_BUS_CONT);
  main226.calibrate(BONK_MAIN226_SHUNT_R, 1.0f);
  // pull down the ALERT line when the current exceeds the limit.
  main226.setShuntVoltageLimit(currentLimit / BONK_MAIN226_SHUNT_R);
  main226.enableShuntOverLimitAlert();

#ifdef BONK_BOOST
  boost226.begin(BONK_BOOST226_ADDRESS);
  boost226.configure(INA226_AVERAGES_4,
		     INA226_BUS_CONV_TIME_140US,
		     INA226_SHUNT_CONV_TIME_140US,
		     INA226_MODE_SHUNT_BUS_CONT);
  boost226.calibrate(BONK_BOOST226_SHUNT_R, 1.0f);
  // current limit supported only for main 
#endif // BONK_BOOST

  writeRegister_9557(REG_9557_INVERT, 0);

  return BONK_HC_OK;
}

enum {
      REG_9557_IN,
      REG_9557_OUT,
      REG_9557_INVERT,
      REG_9557_CONFIG
};

void HardwareControl::containmentPinMode(const uint8_t pin, const boolean isOutput) const {
  containment9557.pinMode(pin, isOutput);
} 

void HardwareControl::containmentDigitalWrite(const uint8_t pin, const boolean isHigh) const {
  containment9557.digitalWrite(pin, isHigh);
}

uint8_t HardwareControl::containmentDigitalRead(const uint8_t pin) const {
  return containment9557.digitalRead(pin);
}

