obj-m := tvp5150.o

PWD := $(shell pwd)

KDIR := /home/linux/ti-processor-sdk-linux-am57xx-evm-04.03.00.05/board-support/linux-4.9.69+gitAUTOINC+9ce43c71ae-g9ce43c71ae
default:
	make -C $(KDIR) M=$(PWD) modules

clean:
	rm *.ko *.mod.*  Module.* module* *.o .t* -rf 
