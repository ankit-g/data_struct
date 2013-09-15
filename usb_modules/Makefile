	obj-m += gad_driver.o udc-core.o dummy_hcd.o

	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE)  -C $(KERNELDIR) M=$(PWD) modules
	@rm -rf *.o  *.mod.c *.mod.o .*.cmd *.order *.symvers
clean:
	@rm *.ko
