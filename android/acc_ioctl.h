#include <linux/ioctl.h>

#define USBDSBL _IO('u', 1) // defines our ioctl call
#define USBENBL _IO('u', 2)
#define USBCMD  _IOW('u', 3, int)	
