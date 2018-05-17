/* 
 * File:   StereoSensor.cpp
 * Author: dcr
 * 
 * Created on June 29, 2016, 9:42 AM
 */

#include "StereoSensor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ABS_DIFF(x, y)     ((x) > (y)) ? ((x)-(y)) : ((y)-(x))
#define MAX(a, b)          (((a)>(b)) ? (a) : (b))

#define VERTICAL_SEARCH_RADIUS      5
#define HORIZONTAL_SEARCH_RADIUS    150
#define DIFF_DISPARITY_THRESHOLD    10

StereoSensor::StereoSensor(DLPC2607* i2c0, DLPC2607* i2c1, CameraInterface* cam0, CameraInterface* cam1)
{
    m_i2c0 = i2c0;
    m_i2c1 = i2c1;
    m_cam0 = cam0;
    m_cam1 = cam1;
    
    m_buffer = (unsigned char*) malloc(320*240);
}


StereoSensor::~StereoSensor() {
}

unsigned char* StereoSensor::captureStereo()
{
    m_cam0->startCaptureFrame();
    m_cam1->startCaptureFrame();
    
    unsigned char* buf0 = m_cam0->finishCaptureFrame();
    unsigned char* buf1 = m_cam1->finishCaptureFrame();
    
    clock_t t0 = clock();
    
    unsigned char* newBuf0 = (unsigned char*) malloc(320*240);
    unsigned char* newBuf1 = (unsigned char*) malloc(320*240);
    memcpy(newBuf0, buf0, 320*240);
    memcpy(newBuf1, buf1, 320*240);
    
    disparityMap(newBuf1, newBuf0, m_buffer);
    
    free(newBuf0);
    free(newBuf1);
    
    clock_t tf = clock();
    
    printf("Disparity Map Computation Took %f seconds\n", (double)((tf-t0)/CLOCKS_PER_SEC));
    
    return m_buffer;
}

unsigned char* StereoSensor::captureDiffStereo()
{
    m_cam0->startCaptureDiffFrame();
    m_cam1->startCaptureDiffFrame();
    
    unsigned char* buf0 = m_cam0->finishCaptureDiffFrame();
    unsigned char* buf1 = m_cam1->finishCaptureDiffFrame();
    
    clock_t t0 = clock();
    
    /*unsigned char* newBuf0 = (unsigned char*) malloc(320*240);
    unsigned char* newBuf1 = (unsigned char*) malloc(320*240);
    memcpy(newBuf0, buf0, 320*240);
    memcpy(newBuf1, buf1, 320*240);*/
    
    disparityMapDiff(buf1, buf0, m_buffer);
    
    //free(newBuf0);
    //free(newBuf1);
    
    clock_t tf = clock();
    
    printf("Disparity Map Diff Computation Took %f seconds\n", (double)((tf-t0)/CLOCKS_PER_SEC));
    
    return m_buffer;
}

void StereoSensor::disparityMap(unsigned char* left, unsigned char* right, unsigned char* dst)
{
    for (int y=0; y < 240; y++)
        for (int x=0; x < 320; x++)
        {
            dst[y*320+x] = findDisparityPoint(left, right, x, y);
        }
}

void StereoSensor::disparityMapDiff(unsigned char* left, unsigned char* right, unsigned char* dst)
{
    for (int y=0; y < 240; y++)
        for (int x=0; x < 320; x++)
        {
            if (left[y*320+x] > DIFF_DISPARITY_THRESHOLD)
                dst[y*320+x] = findDisparityPoint(left, right, x, y);
            else
                dst[y*320+x] = 0;
        }
}

unsigned char StereoSensor::findDisparityPoint(unsigned char* left, unsigned char* right, int xx, int yy)
{
    int sad[11][320];
    
    // compute the SAD of the line up to the xx point of the right image, 
    // considering some (small) degree vertical movement
    for (int y=-VERTICAL_SEARCH_RADIUS; y <= VERTICAL_SEARCH_RADIUS; y++)
        for (int x=MAX(0, xx-HORIZONTAL_SEARCH_RADIUS); x <= xx;  x++)
        {
            int SADV = 0;

            // compare a kernel centered at xx,yy with a kernel centered at x,yy
            for (int iy=-1; iy <= 1; iy++)
                for (int ix=-1; ix <= 1; ix++)
                {
                    int plx = xx + ix;
                    int ply = yy + iy;
                    int prx = x + ix;
                    int pry = yy + y + iy;

                    if ((plx >= 0 && plx < 320)
                        && (prx >= 0 && prx < 320)
                        && (ply >= 0 && ply < 240)
                        && (pry >= 0 && pry < 240))
                    {
                        SADV += ABS_DIFF(left[plx+ply*320], right[prx+pry*320]);
                    }
                }

            sad[y+5][x] = SADV;    
        }
    
    
    // find the minimum value
    int min = sad[0][0];
    int mini = 0;
    
    for (int y=-VERTICAL_SEARCH_RADIUS; y <= VERTICAL_SEARCH_RADIUS; y++)
        for (int i=MAX(0, xx-HORIZONTAL_SEARCH_RADIUS); i < xx; i++)
        {
            if (sad[y+VERTICAL_SEARCH_RADIUS][i] < min)
            {
                mini = i;
                min = sad[y+VERTICAL_SEARCH_RADIUS][i];
            }
        }
    
    //printf("(%d,%d) min = %d\n", xx, yy, mini);
    
    return (unsigned char)((xx - mini) * 255 / HORIZONTAL_SEARCH_RADIUS);    
}