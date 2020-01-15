#KERNEL = `uname -r`
#KERN_DIR = /usr/src/linux-headers-$(shell uname -r)/
KERN_DIR = /home/linux/SDK/AM57xx_RT_05.03.00.07/board-support_rt_5.3.1.3/linux-rt-4.14.79+gitAUTOINC+a72bf1418c-ga72bf1418c
obj-m += mult_time.o

all:
	make -C $(KERN_DIR) M=`pwd` modules


clean:
	rm -rf modules.order *.o* *.mod.* *.symvers .*.cmd *.ko
