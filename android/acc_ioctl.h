#include <linux/ioctl.h>

#define USBDSBL _IO('u', 1) // defines our ioctl call
#define USBENBL _IO('u', 2)
#define USBCMDS _IOW('u', 3, int)	
#define USBCMDR _IOR('u', 4, int)	
#define USBAENB _IO('u', 5)	
