#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "acc_ioctl.h"

#define err_chk(val, func)  do{				\
				if (val <0 ) {		\
					perror(#func);	\
					exit(0);	\
				}			\
			    }while(0)			\

enum usb_controller {
	USB_CMD_SND
};

int main(int argc, char **argv)
{
	int fd;
	int ret;
	enum usb_controller state = atoi(argv[1]);

	int val = atoi(argv[2]);

	fd = open("/dev/usb_accessory_debug",O_RDWR);
	err_chk(fd, open);

	switch (state) {
		case USB_CMD_SND:	
			ret = ioctl(fd, USBCMD, &val);
			err_chk(ret, ioctl);
			break;

		default:
			printf("Currently not handled\n");
	}
		
	return 0;
}
