/**
 * This is a C++ wrapper on http://opencores.org/project,i2c
 * Copyright (c) 2016 David Castells-Rufas 
 *					  david.castells@uab.cat
 * Modified by Vinh to communicate with DLP sensor
 */
 
#include "oc_i2c_master.h"
#include <stdio.h>
#include <unistd.h>
static volatile char v1, v2, v3, v4;
I2CMaster::I2CMaster(void* baseAddress)
{
	m_ba = (int*) baseAddress;
	m_debug = 0;
}


void I2CMaster::setFrequency(double sysFreq, double i2cFreq)
{
	int prescaler = (int)(sysFreq / ( 5 * i2cFreq)) - 1;
	
	int PL = prescaler & 0xFF;
	int PH = prescaler >> 8;
	
	if (m_debug)
		printf("I2C-MASTER: Freq Prescale: %d [0x%02X]- [0x%02X]\n", prescaler, PH, PL);
	m_ba[OC_I2C_PRER_LO] = PL;
	m_ba[OC_I2C_PRER_HI] = PH;
}


void I2CMaster::writeControlRegister(int v)
{
	if (m_debug)
		printf("I2C-MASTER: Control Register= 0x%02X\n", v);
	
	m_ba[OC_I2C_CTR] = v;
}

void I2CMaster::writeCommandRegister(int v)
{
	if (m_debug)
		printf("I2C-MASTER: Command Register= 0x%02X\n", v);
	
	m_ba[OC_I2C_CR] = v;
}


void I2CMaster::writeTransmitRegister(int v)
{
	if (m_debug)
		printf("I2C-MASTER: TX Register= 0x%02X\n", v);
	
	m_ba[OC_I2C_TXR] = v;	
}

int I2CMaster::readReceiveRegister()
{
	int v = m_ba[OC_I2C_RXR] & 0xFF;
	
	if (m_debug)
		printf("I2C-MASTER: RX Register= 0x%02X\n", v);
	
	return v;
}

void I2CMaster::waitEndTransmission()
{
	while (OC_ISSET(m_ba[OC_I2C_SR], OC_I2C_TIP));
}

int I2CMaster::isRxAck()
{
	return OC_ISSET(m_ba[OC_I2C_SR], OC_I2C_RXACK);
}

/**
 *	Reads a byte from a memory address from an I2C slave 
 *  @param device the I2C slave device address
 *	@param address the memory address in the I2C slave device
 *	@param value a pointer where the read value will be stored
 */
void I2CMaster::readByte(char device, char address, char* value)
{
	// First Phase, Write Device Address
	char device_dir = (device << 1) ;//| 1;
	writeTransmitRegister(device_dir);
	writeCommandRegister(OC_I2C_STA | OC_I2C_WR);
	waitEndTransmission();
	
	if (isRxAck())
	{
		if (m_debug) printf("P1 - NO RX NACK\n");
		return;
	}
	
	// Second Phase, Write memory address
	writeTransmitRegister(address);
	writeCommandRegister(OC_I2C_WR);
	waitEndTransmission();
	
	if (isRxAck())
	{
		if (m_debug) printf("P2 - NO RX NACK\n");
		return;
	}
	
	// Third phase, write read address
	device_dir = (device << 1) | 1;
	writeTransmitRegister(device_dir);
	writeCommandRegister(OC_I2C_STA | OC_I2C_WR);
	waitEndTransmission();
	
	writeCommandRegister(OC_I2C_RD | OC_I2C_NACK | OC_I2C_STO);
	
	waitEndTransmission();
	
	*value = readReceiveRegister();
}


void I2CMaster::writeWord(char device, char address, unsigned int value)
{
    // First Phase, Write Device Address
    char device_dir = (device << 1) ;//| 1;
    
    writeTransmitRegister(device_dir);
    writeCommandRegister(OC_I2C_STA | OC_I2C_WR);
    waitEndTransmission();
    
    if (isRxAck())
    {
            if (m_debug) printf("P1 - NO RX NACK\n");
            return;
    }
    
    // Second Phase, Write memory address
    writeTransmitRegister(address);
    writeCommandRegister(OC_I2C_WR);
    waitEndTransmission();

    if (isRxAck())
    {
            if (m_debug) printf("P2 - NO RX NACK\n");
            return;
    }
    
    // Third write highest byte
    int high = (value >> 24) & 0xFF;
    writeTransmitRegister(high);
    writeCommandRegister(OC_I2C_WR);
    waitEndTransmission();

    if (isRxAck())
    {
            if (m_debug) printf("P3 - NO RX NACK\n");
            return;
    }
    
    // Fourth: write second byte
    int second_byte = (value >> 16) & 0xFF;
    writeTransmitRegister(second_byte);
    writeCommandRegister(OC_I2C_WR);
    waitEndTransmission();

    if (isRxAck())
    {
            if (m_debug) printf("P4 - NO RX NACK\n");
            return;
    }

    // Fifth: Write third byte
    int third_byte = (value >> 8) & 0xFF;
    writeTransmitRegister(third_byte);
    writeCommandRegister(OC_I2C_WR);
    waitEndTransmission();

    if (isRxAck())
    {
            if (m_debug) printf("P5 - NO RX NACK\n");
            return;
    }

    // Sixth: write lowest byte
    int low = (value & 0xFF);
    writeTransmitRegister(low);
    writeCommandRegister(OC_I2C_WR);
    waitEndTransmission();

    if (isRxAck())
    {
            if (m_debug) printf("P6 - NO RX NACK\n");
            return;
    }
    
    writeCommandRegister( OC_I2C_STO);
    
    usleep(10);
}

/**
 *	Reads a byte from a memory address from an I2C slave 
 *  @param device the I2C slave device address
 *	@param address the memory address in the I2C slave device
 *	@param value a pointer where the read value will be stored
 */
void I2CMaster::readWord(char device, char address, int* value)
{
	// First Phase, Write Device Address
	char device_dir = (device << 1) ;//| 1;
	writeTransmitRegister(device_dir);
	writeCommandRegister(OC_I2C_STA | OC_I2C_WR);
	waitEndTransmission();
	
	if (isRxAck())
	{
		if (m_debug) printf("P1 - NO RX NACK\n");
		return;
	}
	
	// Second Phase, Write 0x15
	writeTransmitRegister((0x15));
	writeCommandRegister(OC_I2C_WR);
	waitEndTransmission();
	
	if (isRxAck())
	{
		if (m_debug) printf("P2 - NO RX NACK\n");
		return;
	}

	// Third Phase, Write register address
	writeTransmitRegister(address);
	writeCommandRegister(OC_I2C_WR);
	waitEndTransmission();
	
	if (isRxAck())
	{
		if (m_debug) printf("P3 - NO RX ACK\n");
		return;
	}
	writeCommandRegister( OC_I2C_STO);
	usleep(10);
        
	// Fourth phase, Read command
	device_dir = (device << 1) | 1; 
	writeTransmitRegister(device_dir);
	writeCommandRegister(OC_I2C_STA | OC_I2C_WR);
	waitEndTransmission();

		if (isRxAck())
	{
		if (m_debug) printf("P4 - NO RX ACK\n");
		return;
	}

	writeCommandRegister(OC_I2C_RD | OC_I2C_ACK);
	waitEndTransmission();
        

	v1 = readReceiveRegister();
	
	writeCommandRegister(OC_I2C_RD | OC_I2C_ACK);
	waitEndTransmission();

	v2 = readReceiveRegister();
	
	writeCommandRegister(OC_I2C_RD | OC_I2C_ACK);
	waitEndTransmission();
	
	v3 = readReceiveRegister();

	writeCommandRegister(OC_I2C_RD | OC_I2C_NACK | OC_I2C_STO);
	waitEndTransmission();

	v4 = readReceiveRegister();
	
	*value = (v1 << 24) | (v2<<16) | (v3<<8) | v4;
        usleep(10);
	
}
void I2CMaster::setDebug(int v)
{
	m_debug = v;
}

void I2CMaster::enable(int v)
{
	writeControlRegister((v)? OC_I2C_EN : 0);
}
