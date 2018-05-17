/**
 * This is an I2C driver to control the MT9P031 image sensor registers
 *
 * Copyright (c) 2016 David Castells-Rufas 
 *					  david.castells@uab.cat
 */
#ifndef MT9P031_H_INCLUDED_
#define MT9P031_H_INCLUDED_

#include "oc_i2c_master.h"

#define I2C_ADDRESS	27

#define MT9P031_REG_ROW_START               0x01
#define MT9P031_REG_COLUMN_START            0x02
#define MT9P031_REG_ROW_SIZE                0x03
#define MT9P031_REG_COLUMN_SIZE             0x04
#define MT9P031_REG_OUTPUT_CONTROL          0x07
#define MT9P031_REG_SHUTTER_WIDTH_UPPER     0x08
#define MT9P031_REG_SHUTTER_WIDTH_LOWER     0x09
#define MT9P031_REG_PIXEL_CLOCK_CONTROL     0x0A
#define MT9P031_REG_SHUTTER_DELAY           0x0C
#define MT9P031_REG_RESET                   0x0D
#define MT9P031_REG_READ_MODE1              0x1E
#define MT9P031_REG_READ_MODE2              0x20
#define MT9P031_REG_ROW_ADDRESS_MODE        0x22
#define MT9P031_REG_COLUMN_ADDRESS_MODE     0x23
#define MT9P031_REG_GLOBAL_GAIN             0x35


#define MT9P031_FULL_RESOLUTION_X           2592
#define MT9P031_FULL_RESOLUTION_Y           1944

class DLPC2607
{
public:
	DLPC2607(I2CMaster* i2c);
	
	void dumpRegisters();
	unsigned int readRegister(int v);
	void writeRegister(int r, int unsigned v);
        
        void setRowAddressMode(int bin, int skip);
        void setColumnAddressMode(int bin, int skip);
        void setColumnStart(int v);
        void setRowStart(int v);
        void setRowSize(int size);
        void setColumnSize(int size);
        void setVGA();
        void setShutterWidth(int w);
        void setReadMode(int xorLineValid, int continuousLineValid, 
            int invertTrigger, int snapshot, int globalReset, int bulbExposure,
            int invertStrobe, int strobeEnable, int strobeStart, int strobeEnd,
            int mirrorRow, int mirrorColumn, int showDarkColumns, int showDarkRows,
            int rowBlackLevelCompensation, int columnSum);
        
        void setGlobalGain(int digitalGain, int analogMultiplier, int analogGain);
        void setShutterDelay(int delay);
        void setPixelClockControl(int invertPixelClock, int shiftPixelClock, int dividePixelClock);
        
        
protected:
	I2CMaster* m_i2c;
};

#endif
