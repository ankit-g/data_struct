/* Linux Class Driver to handle android open accessory.
 * Initially trying to enable android accessory in android device.
 */

#include "comm.h"
#include <linux/usb.h>
#define NAME "usb_accessory_debug"

static struct cdev cdev;
static dev_t dev;
static struct device *pdev;
static struct usb_device *acc_device;

static ssize_t usbg_read (struct file *, char __user *, size_t, loff_t *);

static struct file_operations fops = {
        .owner          = THIS_MODULE,
        .read           = usbg_read,
};

ssize_t usbg_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
        debugf();
        return size;
}

/* Table of devices that work with this driver */
static struct usb_device_id usb_acc_table[] =
{
// 0525:a4a0
    { USB_DEVICE(0x0525, 0xA4A0) },
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
        debugf();
        acc_device = interface_to_usbdev(intf);
        return 0;
}


static struct usb_driver usb_acc_driver =
{
        .name           = "usb_acc_driver",
        .probe          = usb_acc_probe,
        .disconnect     = usb_acc_disconnect,
        .id_table       = usb_acc_table,
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
