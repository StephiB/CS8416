#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#ifdef __AVR_ATtiny85__	 || __AVR_ATtiny87__ || __AVR_ATtiny167__		
#include <TinyWireM.h>
#define Wire TinyWireM
#else
#include <Wire.h>
#endif

#include <CS8416.h>

bool CS8416::begin() {
	
	Wire.begin();
	/* Check connection */
	uint8_t deviceid = getDeviceID();
	if ( (deviceid != 0x21) || (deviceid != 0x22) || (deviceid != 0x23) || (deviceid != 0x27) || (deviceid != 0x2F) )
	{
		/* No CS8416 detected ... return false */
		//Serial.println(deviceid, HEX);
		return false;
	}  	
	return true;
}


/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
inline uint8_t CS8416::i2cread(void) {
  #if ARDUINO >= 100
  return Wire.read();
  #else
  return Wire.receive();
  #endif
}

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
inline void CS8416::i2cwrite(uint8_t x) {
  #if ARDUINO >= 100
  Wire.write((uint8_t)x);
  #else
  Wire.send(x);
  #endif
}

/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register
*/
/**************************************************************************/
void CS8416::writeRegister(uint8_t reg, uint8_t value) {

    Wire.beginTransmission(CS8416_WRITE_REG);
    i2cwrite((uint8_t)reg);
    i2cwrite((uint8_t)(value));
    Wire.endTransmission();  
}

/**************************************************************************/
/*!
    @brief  Reads 8-bits from the specified register
*/
/**************************************************************************/
uint8_t CS8416::readRegister(uint8_t reg) {
    Wire.beginTransmission(CS8416_WRITE_REG);
    i2cwrite(reg);
    Wire.endTransmission();
    Wire.requestFrom(CS8416_READ_REG, 1);
    return (i2cread());

}

/**************************************************************************/
/*! 
    @brief  Read the device ID (can be used to check connection)
*/
/**************************************************************************/
uint8_t CS8416::getDeviceID(void) {
  // Check device ID register
  return readRegister(CS8416_ID_VERSION);
}