#! /bin/sh 
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
arm-linux-gnueabihf-gcc tsl2561_crc.c -o tsl2561_test

