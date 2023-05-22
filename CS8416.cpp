#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <Wire.h>
#include <SPI.h>
#include <CS8416.h>

// Constructor when using SPI.
CS8416::CS8416(uint8_t cs) {
	_cs   = cs;
	interface = true;
} 
// Constructor when using i2c.
CS8416::CS8416() {
	interface = false;
}  

/**************************************************************************/
//	Start device
/**************************************************************************/
bool CS8416::begin() {		
	uint8_t revision;
	// Start the appropriate interface.
	if (interface) {
		pinMode(_cs, OUTPUT);
		digitalWrite(_cs, HIGH);
		SPI.begin();
		SPI.setBitOrder(MSBFIRST);
		SPI.setClockDivider(SPI_CLOCK_DIV21); // 4MHz for DUE
		revision = getDeviceID();
		if (revision != 0) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		Wire.begin();
		revision = getDeviceID();
		if (revision != 0) {
			return true;
		}
		else {
			return false;
		}
	}
}
/**************************************************************************/
//	Initiate: Run, I²S, 16bit, de-emphasis filter auto-select on,
//	GPIO0 = TX, GPIO1 = INT, GPIO2 = 96kHz
/**************************************************************************/
void CS8416::initiate() { 	
	
	if (interface) {
		digitalWrite(_cs, LOW);
		SPI.beginTransaction()
		SPI.transfer(CS8416_WRITE_REG);
		SPI.transfer(INCR & 0x00);
		SPI.transfer(0x00);
		SPI.transfer(0x00);
		SPI.transfer((CTRL2_EMPH_CNTL0 & 0x40) | (CTRL2_GPO0_SEL & 0x0B));
		SPI.transfer((CTRL3_GPO1SEL & 0x20) | (CTRL3_GPO2SEL & 0x08));
		SPI.transfer(CTRL4_RUN);
		SPI.transfer(FRMT_I2S | (FRMT_SORES & 0x20));
		SPI.transfer(ERRMASK_UNLOCK);
		SPI.transfer(INT_RERRM);
		SPI.transfer(INT_RERR1);
		SPI.transfer(INT_RERR0);
		digitalWrite(_cs, HIGH);
		SPI.endTransaction();
		delayMicroseconds(20); 		
	}
	else{
		Wire.beginTransmission(CS8416_WRITE_REG);
		i2cwrite(INCR & 0x00);
		i2cwrite(0x00);
		i2cwrite(0x00);
		i2cwrite((CTRL2_EMPH_CNTL0 & 0x40) | (CTRL2_GPO0_SEL & 0x0B));
		i2cwrite((CTRL3_GPO1SEL & 0x20) | (CTRL3_GPO2SEL & 0x08));
		i2cwrite(CTRL4_RUN);
		i2cwrite(FRMT_I2S | (FRMT_SORES & 0x20));
		i2cwrite(ERRMASK_UNLOCK);
		i2cwrite(INT_RERRM);
		i2cwrite(INT_RERR1);
		i2cwrite(INT_RERR0);
		Wire.endTransmission();
	}  	
}
/**************************************************************************/
//	Abstract away platform differences in Arduino wire library
/**************************************************************************/
inline uint8_t CS8416::i2cread(void) {
	#if ARDUINO >= 100
	return Wire.read();
	#else
	return Wire.receive();
	#endif
}
/**************************************************************************/
//	Abstract away platform differences in Arduino wire library
/**************************************************************************/
inline void CS8416::i2cwrite(uint8_t x) {
	#if ARDUINO >= 100
	Wire.write((uint8_t)x);
	#else
	Wire.send(x);
	#endif
}
/**************************************************************************/
//	Writes 8-bits to the specified destination register
/**************************************************************************/
void CS8416::writeRegister(uint8_t reg, uint8_t value) {
	
	if (interface) {
		digitalWrite(_cs, LOW);
		SPI.beginTransaction()
		SPI.transfer(CS8416_WRITE_REG);
		SPI.transfer(reg);
		SPI.transfer(value);
		digitalWrite(_cs, HIGH);
		SPI.endTransaction();
	}
	else{		
		Wire.beginTransmission(CS8416_WRITE_REG);
		i2cwrite((uint8_t)reg);
		i2cwrite((uint8_t)(value));
		Wire.endTransmission();
	}
}
/**************************************************************************/
//	Reads 8-bits from the specified register
/**************************************************************************/
uint8_t CS8416::readRegister(uint8_t reg) {
	uint8_t value;
	if (interface) {
		digitalWrite(_cs, LOW);
		SPI.beginTransaction()
		SPI.transfer(CS8416_READ_REG);
		SPI.transfer(reg);
		digitalWrite(_cs, HIGH);
		delayMicroseconds(20);
		digitalWrite(_cs, LOW);
		value = SPI.transfer(0x00);
		digitalWrite(_cs, HIGH);
		SPI.endTransaction();
		return value;
	}
	else{
		Wire.beginTransmission(CS8416_WRITE_REG);
		i2cwrite(reg);
		Wire.endTransmission();
		Wire.requestFrom(CS8416_READ_REG, 1);
		return (i2cread());
	}
}
/**************************************************************************/
//	Change used input and source for TX
/**************************************************************************/
void CS8416::changeInput(uint8_t num) {
	uint8_t tx = num;
	uint8_t rx = num << 3;
	muteOutput(true);
	if (interface) {
		digitalWrite(_cs, LOW);
		SPI.beginTransaction()
		SPI.transfer(CS8416_WRITE_REG);
		SPI.transfer(CS8416_CTRL4);
		SPI.transfer(CTRL4_RUN | (CTRL4_RXSEL & rx ) | (CTRL4_TXSEL & tx));
		digitalWrite(_cs, HIGH);
		SPI.endTransaction();
		muteOutput(false);
	}
	else {
	Wire.beginTransmission(CS8416_WRITE_REG);
	i2cwrite(CS8416_CTRL4);
	i2cwrite(CTRL4_RUN | (CTRL4_RXSEL & rx ) | (CTRL4_TXSEL & tx));
	Wire.endTransmission();
	muteOutput(false);
	}
}
/**************************************************************************/
//	Automatic clock switching
/**************************************************************************/
void CS8416::clockSwitch(boolean cls) {
	uint8_t h;
	if (!cls){
		h = 0;
	}
	else{
		h = 1;
	}
	uint8_t switchbit = h << 7;
	if (interface) {
		digitalWrite(_cs, LOW);
		SPI.beginTransaction()
		SPI.transfer(CS8416_WRITE_REG);
		SPI.transfer(CS8416_CTRL1);
		SPI.transfer(CTRL1_SWCLK & switchbit);
		digitalWrite(_cs, HIGH);
		SPI.endTransaction();
	}
	else {
	Wire.beginTransmission(CS8416_WRITE_REG);
	i2cwrite(CS8416_CTRL1);
	i2cwrite(CTRL1_SWCLK & switchbit);
	Wire.endTransmission();
	}
}
/**************************************************************************/
//	Mute
/**************************************************************************/
void CS8416::muteOutput(boolean mto) {
	uint8_t h;
	if (!mto){
		h = 0;
	}
	else{
		h = 1;
	}
	uint8_t mutebit = h << 6;
	if (interface) {
		digitalWrite(_cs, LOW);
		SPI.beginTransaction()
		SPI.transfer(CS8416_WRITE_REG);
		SPI.transfer(CS8416_CTRL1);
		SPI.transfer(CTRL1_MUTESAO & mutebit);
		digitalWrite(_cs, HIGH);
		SPI.endTransaction();
	}
	else { 		
		Wire.beginTransmission(CS8416_WRITE_REG);
		i2cwrite(CS8416_CTRL1);
		i2cwrite(CTRL1_MUTESAO & mutebit);
		Wire.endTransmission();
	}
}
/**************************************************************************/
//	Read the device ID (can be used to check connection)
/**************************************************************************/
uint8_t CS8416::getDeviceID(void) {
	return readRegister(CS8416_ID_VERSION);
}
/**************************************************************************/
//
/**************************************************************************/
