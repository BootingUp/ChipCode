#! /bin/sh 
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- JN-open5728_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- JN-open5728.dtb
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage -j4
