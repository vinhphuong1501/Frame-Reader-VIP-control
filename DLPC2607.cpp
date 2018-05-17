#include "DLPC2607.h"
//Vinh modified for DLP communication
#include <stdio.h>
#include <unistd.h>

DLPC2607::DLPC2607(I2CMaster* i2c)
{
	m_i2c = i2c;
}

unsigned int DLPC2607::readRegister(int v)
{
	static int c;
	m_i2c->readWord(I2C_ADDRESS, v, &c);
	//printf ("I2C address %d\n", I2C_ADDRESS);
	return ((unsigned int)c);
}

void DLPC2607::writeRegister(int r, unsigned int v)
{
	m_i2c->writeWord(I2C_ADDRESS, r, v);
}

void DLPC2607::dumpRegisters()
{
	printf("-----------------------------\n");
	printf(" DLPC2607 Registers \n");
	printf("-----------------------------\n");
	printf("[%2d] Source selection:		0x%02X\n", 0, readRegister(11));
        usleep(20);
        printf("[%2d] Resolution selection:	0x%02X\n", 1, readRegister(12));
        usleep(20);
        printf("[%2d] Pixel format:		0x%02X\n", 2, readRegister(13));
        usleep(20);
        printf("[%2d] Led driver enable		0x%02X\n", 3, readRegister(22));
        usleep(20);
        printf("[%2d] HSYNC VSYNC polarity	0x%02X\n", 4, readRegister(175));
        usleep(20);
        printf("[%2d] Video frame rate          0x%02X\n", 5, readRegister(25));	
}