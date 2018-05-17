/* 
 * File:   StereoSensor.h
 * Author: dcr
 *
 * Created on June 29, 2016, 9:42 AM
 */

#ifndef STEREOSENSOR_H
#define	STEREOSENSOR_H

#include "DLPC2607.h"
#include "CameraInterface.h"


class StereoSensor {
public:
    StereoSensor(DLPC2607* i2c0, DLPC2607* i2c1, CameraInterface* cam0, CameraInterface* cam1);
    
    virtual ~StereoSensor();

public:
    unsigned char* captureStereo();
    unsigned char* captureDiffStereo();
    
    void disparityMap(unsigned char* left, unsigned char* right, unsigned char* dst);
    void disparityMapDiff(unsigned char* left, unsigned char* right, unsigned char* dst);
    unsigned char findDisparityPoint(unsigned char* left, unsigned char* right, int xx, int yy);
    
private:
    DLPC2607* m_i2c0;
    DLPC2607* m_i2c1;
    CameraInterface* m_cam0;
    CameraInterface* m_cam1;
    unsigned char* m_buffer;
};

#endif	/* STEREOSENSOR_H */

