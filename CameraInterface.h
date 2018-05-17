/* 
 * File:   CameraInterface.h
 * Author: dcr
 *
 * Created on June 6, 2016, 8:35 PM
 */

#ifndef CAMERAINTERFACE_H
#define	CAMERAINTERFACE_H

#define CAMERA_INTERFACE_REG_COLUMN_WIDTH   0
#define CAMERA_INTERFACE_REG_CONTROL        2
#define CAMERA_INTERFACE_REG_STATUS         4
#define CAMERA_INTERFACE_REG_SUBSAMPLING    6

#define REG_CONTROL_RESET_N                 0x01
#define REG_CONTROL_TRIGGER_N               0x02
#define REG_CONTROL_START                   0x04
#define REG_CONTROL_END                     0x08
#define REG_CONTROL_STANDBY_N               0x10
#define REG_CONTROL_OE                      0x20
#define REG_CONTROL_SNAPSHOT                0x40

class CameraInterface 
{
public:
    CameraInterface(void* baseAddress, unsigned char* frameBuffer);
    
    virtual ~CameraInterface();
    
public:
    void setColumnWidth(int n);
    void setSubsampling(int x, int y, int i);
    void reset(int reset);
    void start(int start);
    void end(int end);
    void doReset();
    void doStart();
    void doEnd();
    void startCaptureFrame();
    unsigned char* finishCaptureFrame();
    unsigned char* captureFrame();
    void startCaptureDiffFrame();
    unsigned char* finishCaptureDiffFrame();
    unsigned char* captureDiffFrame();
    void dumpRegisters();
    
private:

    volatile int* m_ba;
    int m_debug;
    
    unsigned char* m_frameBuffer;
    unsigned char* m_prevFrame;
};

#endif	/* CAMERAINTERFACE_H */

