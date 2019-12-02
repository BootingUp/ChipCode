#! /bin/sh 
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- JN-ccssoft-beta_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- JN-ccssoft-beta.dtb
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage

#make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- JN-mini5728_defconfig
#make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- JN-mini5728.dtb
#make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage
