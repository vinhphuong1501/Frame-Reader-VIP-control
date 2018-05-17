#
TARGET = testdlp

#
HW_DIR=../fpga-rtl/
CROSS_COMPILE = arm-linux-gnueabihf-
CFLAGS = -O3 -static -std=c++11 -g -Wall  -I${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include -I${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include/soc_cv_av
CPPFLAGS = -O3 -static -std=c++11 -g -Wall  -I${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include -I${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include/soc_cv_av
LDFLAGS =  -O3 -g -std=c++11 
CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)g++
ARCH= arm


build: $(TARGET)
#$(TARGET): hps_0.h main.o oc_i2c_master.o DLPC2607.o CameraInterface.o StereoSensor.o
$(TARGET): main.o oc_i2c_master.o DLPC2607.o CameraInterface.o StereoSensor.o Image.o BufferedImage.o 
	$(LD) $(LDFLAGS)   $^ -o $@
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o : %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

#hps_0.h:
#	sopc-create-header-files $(HW_DIR)soc_system.sopcinfo --single hps_0.h --module hps_0
	
download:
	scp $(TARGET) root@158.109.74.188:/home/root
	
upload:
	scp root@158.109.74.188:/home/root/*.bmp .

.PHONY: clean
clean:
	#rm -f hps_0.h
	rm -f $(TARGET) *.a *.o *~ 
