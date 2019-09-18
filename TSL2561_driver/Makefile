EXTRA_CFLAGS=-g -o0

obj-m += tsl2561_driver.o

KVERS = $(shell uname -r)
KEN_DIR=/home/linux/SDK/AM57xx_RT_05.03.00.07/board-support_rt_5.3.1/linux-rt-4.14.79+gitAUTOINC+a72bf1418c-ga72bf1418c

ALL:
	make -C $(KEN_DIR) M=$(CURDIR) modules

clean:
	-rm *.o .t* tsl2561_driver.ko modules.* Module.* *.mod.c -rf
