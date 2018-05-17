/* 
 * File:   CameraInterface.cpp
 * Author: dcr
 * 
 * Created on June 6, 2016, 8:35 PM
 */

#include "CameraInterface.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

CameraInterface::CameraInterface(void* baseAddress, unsigned char* frameBuffer)
{
	m_ba = (int*) baseAddress;
	m_debug = 0;
        m_frameBuffer = frameBuffer;
        m_prevFrame = NULL;
}

CameraInterface::~CameraInterface() 
{
}


/**
 * 
 * @param reset if true it performs a reset into the device
 */
void CameraInterface::reset(int reset)
{
    if (reset)
        m_ba[CAMERA_INTERFACE_REG_CONTROL] = /*REG_CONTROL_START*/ REG_CONTROL_TRIGGER_N /*REG_CONTROL_RESET_N = 0*/;
    else
        m_ba[CAMERA_INTERFACE_REG_CONTROL] = /*REG_CONTROL_START*/ REG_CONTROL_TRIGGER_N | REG_CONTROL_RESET_N;
}

void CameraInterface::start(int start)
{
    if (start)
        m_ba[CAMERA_INTERFACE_REG_CONTROL] = REG_CONTROL_START /* REG_CONTROL_TRIGGER_N*/ | REG_CONTROL_RESET_N ;
    else
        m_ba[CAMERA_INTERFACE_REG_CONTROL] = /*REG_CONTROL_START*/ REG_CONTROL_TRIGGER_N | REG_CONTROL_RESET_N ;
}

void CameraInterface::startCaptureFrame()
{
    m_ba[CAMERA_INTERFACE_REG_CONTROL] = REG_CONTROL_SNAPSHOT | REG_CONTROL_START /* REG_CONTROL_TRIGGER_N*/ | REG_CONTROL_RESET_N ;
    m_ba[CAMERA_INTERFACE_REG_CONTROL] = REG_CONTROL_SNAPSHOT | /*REG_CONTROL_START*/ REG_CONTROL_TRIGGER_N | REG_CONTROL_RESET_N ;

}

unsigned char* CameraInterface::finishCaptureFrame()
{
    while (m_ba[CAMERA_INTERFACE_REG_STATUS] & 0x1)
    {
        //printf(".\n");
    } 
    
    doEnd();
    
    return m_frameBuffer;
}

unsigned char* CameraInterface::captureFrame()
{
    startCaptureFrame();
    return finishCaptureFrame();
}

#define ABS_DIFF(x, y)     ((x) > (y)) ? ((x)-(y)) : ((y)-(x))

void CameraInterface::startCaptureDiffFrame()
{
    if (m_prevFrame == NULL)
        m_prevFrame = (unsigned char*) malloc(320*240);
    
    // save the previous frame
    memcpy(m_prevFrame, m_frameBuffer, 320*240);
    
    // capture a new frame
    startCaptureFrame();
}

unsigned char* CameraInterface::finishCaptureDiffFrame()
{
    finishCaptureFrame();
    
    // compute the difference
    for (int i=0; i < (320*240); i++)
        m_prevFrame[i] = ABS_DIFF(m_prevFrame[i], m_frameBuffer[i]);
    
    return m_prevFrame;
}

unsigned char* CameraInterface::captureDiffFrame()
{
    startCaptureDiffFrame();
    return finishCaptureDiffFrame();
}

void CameraInterface::setSubsampling(int x, int y, int i)
{
    m_ba[CAMERA_INTERFACE_REG_SUBSAMPLING] = i << 4 | x << 2 | y;
}

void CameraInterface::dumpRegisters()
{
    	printf("-----------------------------\n");
	printf(" CameraInterface Registers \n");
	printf("-----------------------------\n");
        
	printf("[%2d] Column Width: 		0x%08X\n", CAMERA_INTERFACE_REG_COLUMN_WIDTH, m_ba[CAMERA_INTERFACE_REG_COLUMN_WIDTH]);
	printf("[%2d] Control:                  0x%08X\n", CAMERA_INTERFACE_REG_CONTROL, m_ba[CAMERA_INTERFACE_REG_CONTROL]);
        printf("[%2d] Status:                   0x%08X\n", CAMERA_INTERFACE_REG_STATUS, m_ba[CAMERA_INTERFACE_REG_STATUS]);
        printf("[%2d] Subsampling: 		0x%08X\n", CAMERA_INTERFACE_REG_SUBSAMPLING, m_ba[CAMERA_INTERFACE_REG_SUBSAMPLING]);
        
}

void CameraInterface::end(int end)
{
    if (end)
        m_ba[CAMERA_INTERFACE_REG_CONTROL] = REG_CONTROL_END | REG_CONTROL_TRIGGER_N | REG_CONTROL_RESET_N ;
    else
        m_ba[CAMERA_INTERFACE_REG_CONTROL] = /*REG_CONTROL_START*/ REG_CONTROL_TRIGGER_N | REG_CONTROL_RESET_N ;
}

void CameraInterface::doReset()
{
    reset(1);
    usleep(20);
    reset(0);
}

void CameraInterface::doStart()
{
    start(1);
    start(0);
}

void CameraInterface::doEnd()
{
    end(1);
    end(0);
}

void CameraInterface::setColumnWidth(int w)
{
    m_ba[CAMERA_INTERFACE_REG_COLUMN_WIDTH] = w;
}
