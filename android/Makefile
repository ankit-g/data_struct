	obj-m += accessory.o 
	accessory-objs := usb_accessory.o char_core.o

	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE)  -C $(KERNELDIR) M=$(PWD) modules
	@rm -rf *.o  *.mod.c .*.cmd *.order *.symvers
clean:
	@rm *.ko
