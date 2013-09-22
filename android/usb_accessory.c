/* Linux Class Driver to handle android open accessory.
 * Initially trying to enable android accessory in android device.
 */

#include "comm.h"
#include "acc_ioctl.h"
#include <linux/usb.h>
#include <linux/uaccess.h>
#define NAME "usb_accessory_debug"

static struct cdev cdev;
static dev_t dev;
static struct device *pdev;
static struct usb_device *acc_device;

static ssize_t usbg_read (struct file *, char __user *, size_t, loff_t *);
static long usbg_ioctl(struct file *, unsigned int, unsigned long);

static struct file_operations fops = {
        .owner          = THIS_MODULE,
        .read           = usbg_read,
	.unlocked_ioctl = usbg_ioctl
};

ssize_t usbg_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
        debugf();
        return size;
}

static long usbg_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
        int ret = 0;
	int usb_cmd = 0;
	int data = 100;
	
	char str[] = "Ankyt-G";

        switch (cmd) {

        case USBCMDS: if (copy_from_user(&usb_cmd, (int __user *)arg, sizeof(int))) {
			 	printk(KERN_ERR"Unable to copy from user\n");
			 	ret =  -EFAULT;
		     	 	goto err_fault;
		      }	

		      ret = usb_control_msg(acc_device, usb_sndctrlpipe(acc_device, 0),
                       	    52, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                            0, 0, str, sizeof(str), 100);		

		      ret = usb_control_msg(acc_device, usb_sndctrlpipe(acc_device, 0),
                       	    52, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                            0, 1, str, sizeof(str), 100);		

		      ret = usb_control_msg(acc_device, usb_sndctrlpipe(acc_device, 0),
                       	    52, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                            0, 2, str, sizeof(str), 100);		

		      ret = usb_control_msg(acc_device, usb_sndctrlpipe(acc_device, 0),
                       	    52, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                            0, 3, str, sizeof(str), 100);		
		     
		      ret = usb_control_msg(acc_device, usb_sndctrlpipe(acc_device, 0),
                       	    52, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                            0, 4, str, sizeof(str), 100);		

		      ret = usb_control_msg(acc_device, usb_sndctrlpipe(acc_device, 0),
                       	    52, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                            0, 5, str, sizeof(str), 100);		
	
		      ret = usb_control_msg(acc_device, usb_sndctrlpipe(acc_device, 0),
                       	    53, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT,
                            usb_cmd, 0, NULL, 0, 100);		

        	      break;
        
	case USBCMDR: ret = usb_control_msg(acc_device, usb_rcvctrlpipe(acc_device, 0),
                       	    51, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_IN,
                            usb_cmd, 0, (void *)&data, sizeof(int), 100);		

		      if (copy_to_user((int __user *)arg, &data, sizeof(int))) {
		      		printk(KERN_ERR"Unable to copy to user\n");
			 	ret =  -EFAULT;
		     	 	goto err_fault;
		      }
		      break;	 

	case USBAENB: ret = usb_control_msg(acc_device, usb_rcvctrlpipe(acc_device, 0),
                            USB_REQ_SET_CONFIGURATION, USB_RECIP_DEVICE | USB_DIR_OUT,
                            1, 0, NULL, 0, 100); 
        	      break;                       

	default:      debug("Command not found");	
        }

err_fault:
        return ret;

}

/* Table of devices that work with this driver */
static struct usb_device_id usb_acc_table[] = {
 // 0525:a4a0
    { USB_DEVICE(0x0525, 0xA4A0) },
    { USB_DEVICE(0x04e8, 0x6860) },
    { USB_DEVICE(0x18d1, 0x2d01) },
    { USB_DEVICE(0x04e8, 0x685c) },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, usb_acc_table);


static void usb_acc_disconnect(struct usb_interface *interface)
{
	debugf();
}


int usb_acc_probe (struct usb_interface *intf,
                      const struct usb_device_id *id)
{
	int ret = 0;
        debugf();
        acc_device = interface_to_usbdev(intf);

        return ret;
}


static struct usb_driver usb_acc_driver =
{
        .name           = "usb_acc_driver",
        .probe          = usb_acc_probe,
        .disconnect     = usb_acc_disconnect,
        .id_table       = usb_acc_table
};


static int create_device(void)
{
	int ret = 0;

        pdev = crt_debug_device(&fops, &cdev, &dev, NAME);
        if (IS_ERR(pdev)) {
                printk(KERN_ERR"unable create device \n");
                ret = -ENOMEM;
        }
	
	return ret;
}

static int __init usb_accessory_init(void)
{
	int ret = 0;
	debugf();

	if((ret = create_device()))
	goto err_dev;

	return usb_register(&usb_acc_driver);

err_dev:
	return ret;
}

module_init(usb_accessory_init);

static void __exit usb_accessory_exit(void)
{
	debugf();
	remove_device(&dev, &cdev);
	usb_deregister(&usb_acc_driver);
}

module_exit(usb_accessory_exit);

MODULE_AUTHOR("Ankit Gupta <ankitgupta.work@gmail.com>");
MODULE_DESCRIPTION("USB Class module to enable f_accessory in device");
MODULE_LICENSE("GPL");
