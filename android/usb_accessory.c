/* Linux Class Driver to handle android open accessory.
 * Initially trying to enable android accessory in android device.
 */

#include "comm.h"
#define NAME "usb_accessory_debug"

static struct cdev cdev;
static dev_t dev;
static struct device *pdev;

static ssize_t usbg_read (struct file *, char __user *, size_t, loff_t *);

static struct file_operations fops = {
        .owner          = THIS_MODULE,
        .read           = usbg_read,
};


ssize_t usbg_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
        debug("NULL\n");
        return size;
}



static int __init usb_accessory_init(void)
{
	int ret = 0;
	
	debug("%s\n", __func__);

	pdev = crt_debug_device(&fops, &cdev, &dev, NAME);	
	if (IS_ERR(pdev)) {
                printk(KERN_ERR"unable create device \n");
                return -1;
        }
	

	return ret;
}

module_init(usb_accessory_init);

static void __exit usb_accessory_exit(void)
{
	debug("%s \n", __func__);
	remove_device(&dev, &cdev);
}

module_exit(usb_accessory_exit);

MODULE_AUTHOR("Ankit Gupta <ankitgupta.work@gmail.com>");
MODULE_DESCRIPTION("USB Class module to enable f_accessory in device");
MODULE_LICENSE("GPL");
