#ifndef BONK_HARDWARE_CONTROL_H_
#define BONK_HARDWARE_CONTROL_H_

#ifndef BONK_9557_ADDRESS
#define BONK_9557_ADDRESS 0b1111111;
#endif
#ifndef BONK_MAIN226_ADDRESS
#define BONK_MAIN226_ADDRESS 0b1000000
#endif
#ifndef BONK_BOOST226_ADDRESS
#define BONK_BOOST226_ADDRESS 0b1000101
#endif
#ifndef BONK_MAIN226_SHUNT_R
#define BONK_MAIN226_SHUNT_R 0.05f
#endif
#ifndef BONK_BOOST226_SHUNT_R
#define BONK_BOOST226_SHUNT_R 0.05f
#endif
#ifndef BONK_BOOST_ENABLE_PIN
#define BONK_BOOST_ENABLE_PIN 2
#endif

namespace BONK {

  typedef enum HardwareControlError {
				     BONK_HC_OK,
				     BONK_HC_9557_NOT_PRESENT,
				     BONK_HC_MAIN226_NOT_PRESENT,
				     BONK_HC_BOOST226_NOT_PRESENT,
  } HardwareControlError;

  class HardwareControl {
  public:
    HardwareControl(): main226(INA226()),
#ifdef BONK_BOOST
		       boost226(INA226())
#endif
    { }
    HardwareControlError begin(const float currentLimit);
    void containmentPinMode(const uint8_t pin, const boolean) const;
    void containmentDigitalWrite(const uint8_t pin, const boolean) const;
    boolean containmentDigitalRead(const uint8_t pin) const;
    float readMainCurrent() const { return main226.readShuntCurrent(); } // in milliamps
    float readMainVoltage() const { return main226.readBusVoltage(); } // in millivolts
    float readMainPower() const { return main226.readBusPower(); } // in milliwatts

#ifdef BONK_BOOST
    void enableBoostConverter(const boolean) const;
    float readBoostCurrent() const { return boost226.readShuntCurrent(); }
    float readBoostVoltage() const { return boost226.readBusVoltage(); }
    float readBoostPower() const { return boost226.readBusPower(); }
#endif
//    INA226 get_main226() const { return main226; }
//    INA226 get_boost226() const { return boost226; }
  private:
    Bonk_9557 containment9557;
    INA226 main226;
#ifdef BONK_BOOST
    INA226 boost226;
#endif
  }

    class Bonk_9557 {
    public:
      Bonk_9557(uint8_t addr) {
			       registerCache[0] = 0;
			       // this will be reset after begin(), though.
			       registerCache[1] = 0b11110000;
			       registerCache[2] = 0xFF;
			       address = addr;
      }

      void begin() {
	writeRegister(REG_9557_INVERT, 0);
      }

      uint8_t pinMode(const uint8_t pin, const boolean isOutput) {
	uint8_t oldConfig = readRegister(REG_9557_CONFIG);
	uint8_t newConfig = isOutput ?
	  oldConfig & ~(1<<pin) :
	  oldConfig | (1<<pin);
	writeRegister(REG_9557_CONFIG, newConfig);
	return BONK_HC_OK;
      }

      uint8_t digitalWrite(const uint8_t pin, const boolean isHigh) {
	uint8_t oldOut = readRegister(REG_9557_OUTPUT);
	uint8_t newOut = isHigh ?
	  oldOut | (1 << pin) :
	  oldOut & ~(1 << pin);
	writeRegister(REG_9557_OUTPUT, newOut);
	return BONK_HC_OK;
      }

      // TODO: returning the error?
      uint8_t digitalRead(const uint8_t pin) {
	return (readPins() >> pin) & 1;
      }
    private:
      uint8_t address;
      // we do not cache the input register, so the first element is the output register.
      uint8_t registerCache[3];
      uint8_t writeRegister(uint8_t reg, uint8_t data) {
	// TODO: allow alternate wire, in case somebody insane wants to use the main i2c interface for
	// something else (eg, a camera), though they should *really* be trying to use the extra USART as
	// an I2C in that case.
	if (reg > REG_9557_IN && registerCache[reg] == data) {
	  return BONK_HC_OK;
	}
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(data);
	uint8_t error = Wire.endTransmission();
	if (error == 0) {
	  registerCache[reg] = data;
	}
	return error;
      }
      uint8_t readPins() {
	Wire.beginTransmission(address);
	Wire.write(REG_9557_IN);
	Wire.endTransmission();
	Wire.requestFrom(address, 1);
	return Wire.read();
      }
      // for reg > 0
      uint8_t readRegister(const uint8_t reg) const {
	return registerCache[reg];
      }
    }

}

#endif // HARDWARE_CONTROL_H_
