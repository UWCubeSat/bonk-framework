#ifndef BONK_HARDWARE_CONTROL_H_
#define BONK_HARDWARE_CONTROL_H_

#include <Wire.h>
#include <INA226.h>

#define BONK_CONTAINMENT9557_ADDRESS 0b1111111;
#define BONK_MAIN226_ADDRESS 0b1000000
#define BONK_BOOST226_ADDRESS 0b1000101
#ifndef BONK_BOOST_ENABLE_PIN
#define BONK_BOOST_ENABLE_PIN 2
#endif

namespace Bonk {

	class Main226: private INA226 {
	public:
		void begin(float shunt_resistor) {
			INA226::begin();
			INA226::configure(
				INA226_AVERAGES_4,
				INA226_BUS_CONV_TIME_140US,
				INA226_SHUNT_CONV_TIME_140US,
				INA226_MODE_SHUNT_BUS_CONT);
			// it's unlikely we'll need precise current readings, so I'd rather go high
			// with the maximum current to make it easier to debug overcurrents
			INA226::calibrate(shunt_resistor, 2);
		}
		void begin() {
			begin(0.05f);
		}
	};

	class Boost226: private INA226 {
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
	}

	enum pca9557_register {
		PCA9557_INPUT,
		PCA9557_OUTPUT,
		PCA9557_INVERT,
		PCA9557_CONFIG,
	};

	class Pca9557 {
	public:
		Pca9557(uint8_t addr) {
			registerCache[0] = 0;
			// this will be reset after begin(), though.
			registerCache[1] = 0b11110000;
			registerCache[2] = 0xFF;
			address = addr;
		}

		void begin() {
			writeRegister(PCA9557_INVERT, 0);
		}

		void pinMode(const uint8_t pin, const boolean isOutput) {
			uint8_t oldConfig = readRegister(PCA9557_CONFIG);
			uint8_t newConfig = isOutput ?
				oldConfig & ~(1<<pin) :
				oldConfig | (1<<pin);
			writeRegister(PCA9557_CONFIG, newConfig);
		}

		void digitalWrite(const uint8_t pin, const boolean isHigh) {
			uint8_t oldOut = readRegister(PCA9557_OUTPUT);
			uint8_t newOut = isHigh ?
				oldOut | (1 << pin) :
				oldOut & ~(1 << pin);
			writeRegister(PCA9557_OUTPUT, newOut);
		}

		uint8_t digitalRead(const uint8_t pin) const {
			return (readPins() >> pin) & 1;
		}
	private:
		uint8_t address;
		// we do not cache the input register, so the first element is the output register.
		uint8_t registerCache[3];
		void writeRegister(uint8_t reg, uint8_t data) {
			// TODO: allow alternate wire, in case somebody insane wants to use the main i2c interface for
			// something else (eg, a camera), though they should *really* be trying to use the extra USART as
			// an I2C in that case.
			if (reg > REG_9557_IN && registerCache[reg] == data) {
				// violating cse 143 guidelines: check!
				return;
			}
			Wire.beginTransmission(address);
			Wire.write(reg);
			Wire.write(data);
			// TODO: errors, here and on all other endTransmissions
			Wire.endTransmission();
			registerCache[reg] = data;
		}
		uint8_t readPins() const {
			Wire.beginTransmission(address);
			Wire.write(PCA9557_INPUT);
			Wire.endTransmission();
			Wire.requestFrom(address, 1);
			return Wire.read();
		}
		// for reg > 0
		uint8_t readRegister(const uint8_t reg) const {
			return registerCache[reg];
		}
	};

	enum tmp411_resolution {
		TMP411_RESOLUTION_9BIT,
		TMP411_RESOLUTION_10BIT,
		TMP411_RESOLUTION_11BIT,
		TMP411_RESOLUTION_12BIT,
	};

	enum tmp411_conversion_rate {
		TMP411_RATE_16S, // read: 16 seconds from the beginning of one conversion to the
                    		 // beginning of the next conversion.
		TMP411_RATE_8S,
		TMP411_RATE_4S,
		TMP411_RATE_2S,
		TMP411_RATE_1S,
		TMP411_RATE_S5, // .5 seconds
		TMP411_RATE_S25,
		TMP411_RATE_S125, // still uses less than half a milliamp.
	};

	// not really sure why to make it an enum when every value is specified manually; oh well
	enum tmp411_register {
		TMP411_LOCAL_TEMP_H  = 0x00,
		TMP411_LOCAL_TEMP_L  = 0x15,
		TMP411_REMOTE_TEMP_H = 0x01,
		TMP411_REMOTE_TEMP_L = 0x10,
		TMP411_STATUS        = 0x02,
		// for writing only:
		TMP411_CONFIG_W      = 0x09, 
		TMP411_CONV_RATE_W   = 0x0A,
		TMP411_RESOLUTION_W  = 0x1A,
	};
  
	class Tmp411 {
	public:
		Tmp411(uint8_t addr): _addr(addr) { }
		void begin(bool extendedRange, uint8_t resolution, uint8_t conversionRate) {
			writeRegister(TMP411_CONFIG_W, (1 << 7) | (extendedRange * (1 << 2)));
			writeRegister(TMP411_CONV_RATE_W, conversionRate);
			writeRegister(TMP411_RESOLUTION_W, resolution);
		}
		// shift right by 8 bits to get the temperature in celsius.
		uint16_t readLocalTemperature() {
			return readRegister16(TMP411_LOCAL_TEMP_H, TMP411_LOCAL_TEMP_L);
		}
		uint16_t readRemoteTemperature() {
			return readRegister16(TMP411_REMOTE_TEMP_H, TMP411_REMOTE_TEMP_L);
		}
	private:
		uint8_t _addr;
		void writeRegister(uint8_t reg, uint8_t val) {
			Wire.beginTransmission(_addr);
			Wire.write(reg);
			Wire.write(val);
			Wire.endTransmission();
		}
		uint8_t readRegister(uint8_t reg) {
			Wire.beginTransmission(_addr);
			Wire.write(reg);
			Wire.endTransmission();
			Wire.requestFrom(_addr, 1);
			return Wire.read();
		}
		uint16_t readRegister16(uint8_t regH, uint8_t regL) {
			return (readRegister(regH) << 8) + readRegister(regL);
		}
	};
}

#endif // HARDWARE_CONTROL_H_
