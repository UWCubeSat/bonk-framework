#ifndef BONK_HARDWARE_CONTROL_H
#define BONK_HARDWARE_CONTROL_H

#include <Wire.h>
#include <INA226.h>

#define BONK_CONTAINMENT9557_ADDRESS 0b0011000
#define BONK_MAIN226_ADDRESS 0b1000000
#define BONK_BOOST226_ADDRESS 0b1000101
#ifndef BONK_BOOST_ENABLE_PIN
#define BONK_BOOST_ENABLE_PIN 2
#endif

namespace Bonk {

	class Main226: public INA226 {
	public:
		void begin(float shuntResistor, float currentLimit) {
			INA226::begin();
			INA226::configure(
				INA226_AVERAGES_4,
				INA226_BUS_CONV_TIME_140US,
				INA226_SHUNT_CONV_TIME_140US,
				INA226_MODE_SHUNT_BUS_CONT);
			// it's unlikely we'll need precise current readings, so I'd rather go high
			// with the maximum current to make it easier to debug overcurrents
			INA226::calibrate(shuntResistor, 2.0f);
			INA226::setShuntVoltageLimit(currentLimit * shuntResistor);
			INA226::enableShuntOverLimitAlert();
		}
		void begin() {
			Main226::begin(0.05f, 0.9f);
		}
	};

	class Boost226: public INA226 {
	public:
		void begin(float shunt_resistor) {
			INA226::begin(BONK_BOOST226_ADDRESS);
			INA226::configure(
				INA226_AVERAGES_4,
				INA226_BUS_CONV_TIME_140US,
				INA226_SHUNT_CONV_TIME_140US,
				INA226_MODE_SHUNT_BUS_CONT);
			// boost should have substantially lower current
			INA226::calibrate(shunt_resistor, 1);
		}
		void begin() {
			begin(0.05f);
		}
	};

	void enableBoostConverter(bool enable) {
		digitalWrite(BONK_BOOST_ENABLE_PIN, enable);
		pinMode(BONK_BOOST_ENABLE_PIN, OUTPUT);
	}

	enum class Pca9557Register {
		INPUT,
		OUTPUT,
		INVERT,
		CONFIG,
	};

	class Pca9557 {
	public:
		Pca9557(uint8_t addr) {
			registerCache[0] = 0;
			// this will be reset after begin(), though.
			registerCache[1] = 0b11110000;
			registerCache[2] = 0xFF;
			_addr = addr;
		}

		void begin() {
			writeRegister(Pca9557Register::INVERT, 0);
		}

		void pinMode(const uint8_t pin, const boolean isOutput) {
			uint8_t oldConfig = readRegister(Pca9557Register::CONFIG);
			uint8_t newConfig = isOutput ?
				oldConfig & ~(1<<pin) :
				oldConfig | (1<<pin);
			writeRegister(Pca9557Register::CONFIG, newConfig);
		}

		void digitalWrite(const uint8_t pin, const boolean isHigh) {
			uint8_t oldOut = readRegister(Pca9557Register::OUTPUT);
			uint8_t newOut = isHigh ?
				oldOut | (1 << pin) :
				oldOut & ~(1 << pin);
			writeRegister(Pca9557Register::OUTPUT, newOut);
		}

		uint8_t digitalRead(const uint8_t pin) const {
			return (readPins() >> pin) & 1;
		}
	private:
		uint8_t _addr;
		// we do not cache the input register, so the first element is the output register.
		uint8_t registerCache[3];
		void writeRegister(const Pca9557Register reg, const uint8_t data) {
			// TODO: allow alternate wire, in case somebody insane wants to use the main i2c interface for
			// something else (eg, a camera), though they should *really* be trying to use the extra USART as
			// an I2C in that case.
			if (reg > Pca9557Register::INPUT && registerCache[(uint8_t)reg - 1] == data) {
				// violating cse 143 guidelines: check!
				return;
			}
			Wire.beginTransmission(_addr);
			Wire.write((uint8_t)reg);
			Wire.write(data);
			// TODO: errors, here and on all other endTransmissions
			Wire.endTransmission();
			registerCache[reg - 1] = data;
		}
		uint8_t readPins() const {
			Wire.beginTransmission(_addr);
			Wire.write((uint8_t)Pca9557Register::INPUT);
			Wire.endTransmission();
			Wire.requestFrom(_addr, 1);
			return Wire.read();
		}
		// for reg > 0
		uint8_t readRegister(const Pca9557Register reg) const {
			return registerCache[(uint8_t)reg - 1];
		}
	};

	enum class Tmp411Resolution {
		RESOLUTION_9BIT,
		RESOLUTION_10BIT,
		RESOLUTION_11BIT,
		RESOLUTION_12BIT,
	};

	enum class Tmp411ConversionRate {
		RATE_16S, // read: 16 seconds from the beginning of one conversion to the
		          // beginning of the next conversion.
		RATE_8S,
		RATE_4S,
		RATE_2S,
		RATE_1S,
		RATE_S5,  // .5 seconds
		RATE_S25,
		RATE_S125, // 8 convs/sec, still uses less than half a milliamp.
	};

	// not really sure why to make it an enum when every value is specified manually; oh well
	enum class Tmp411Register {
		LOCAL_TEMP    = 0x00,
		REMOTE_TEMP   = 0x01,
		STATUS        = 0x02,
		// for writing only:
		CONFIG_W      = 0x09,
		CONV_RATE_W   = 0x0A,
		RESOLUTION_W  = 0x1A,
	};
  
	class Tmp411 {
	public:
		Tmp411(uint8_t addr): _addr(addr) { }
		void begin(bool extendedRange,
			   Tmp411Resolution resolution,
			   Tmp411ConversionRate conversionRate) {
			writeRegister(Tmp411Register::CONFIG_W, (1 << 7) | (extendedRange * (1 << 2)));
			writeRegister(Tmp411Register::CONV_RATE_W, (uint8_t)conversionRate);
			writeRegister(Tmp411register::RESOLUTION_W, (uint8_t)resolution);
		}
		void begin() {
			Tmp411::begin(false,
				      Tmp411Resolution::RESOLUTION_12BIT,
				      Tmp411ConversionRate::RATE_S125);
		}
		// shift right by 8 bits to get the temperature in celsius.
		uint16_t readLocalTemperature() {
			return readRegister16(Tmp411Register::LOCAL_TEMP);
		}
		uint16_t readRemoteTemperature() {
			return readRegister16(Tmp411Register::REMOTE_TEMP);
		}
	private:
		uint8_t _addr;
		void writeRegister(Tmp411Register reg, uint8_t val) {
			Wire.beginTransmission(_addr);
			Wire.write((uint8_t)reg);
			Wire.write(val);
			Wire.endTransmission();
		}
		uint8_t readRegister(Tmp411Register reg) {
			Wire.beginTransmission(_addr);
			Wire.write((uint8_t)reg);
			Wire.endTransmission();
			Wire.requestFrom(_addr, 1);
			return Wire.read();
		}
		uint16_t readRegister16(Tmp411Register reg) {
			Wire.beginTransmission(_addr);
			Wire.write((uint8_t)reg);
			Wire.endTransmission();
			Wire.requestFrom(_addr, 2);
			return (Wire.read() << 8) + Wire.read();
		}
	};
}

#endif // HARDWARE_CONTROL_H_
