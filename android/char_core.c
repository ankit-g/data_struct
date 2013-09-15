#include "comm.h"

#define CLASS_NAME "usb_accessory_android"
#define BASE_MINOR 0
#define MINOR_COUNT 1

static struct class *debug_class = NULL;
static int dev_count = 0;

int create_char_debug_class(void)
{
	debug_class = class_create(THIS_MODULE, CLASS_NAME);
        if (IS_ERR(debug_class)) {
                printk(KERN_ERR"unable to create class\n");
                return PTR_ERR(debug_class);
        }

	return 0;
}

void cln_debug_class(void)
{
	class_destroy(debug_class);
        debug_class = NULL;
}

//Creates a debug character device and a node with provided file operations.

struct device *crt_debug_device(const struct file_operations *fops, struct cdev *cdev, 
dev_t *dev, const char *name)
{
	int ret = 0;
	struct device *pdev;

	if (!debug_class) 
		if (create_char_debug_class())		
			goto error_reg; 		

	ret = alloc_chrdev_region(dev, BASE_MINOR, MINOR_COUNT, name);
        if (ret != 0) {
                printk(KERN_ALERT "alloc_chrdev failed!!!\n");
		goto error_reg; 		
        }

	(*cdev).owner = THIS_MODULE;
        cdev_init(cdev, fops);

	ret = cdev_add(cdev, *dev, MINOR_COUNT);
        if (ret) {
                printk(KERN_ALERT "cdev_add failed!!!\n");
		goto error_reg; 		
        }


	pdev = device_create(debug_class, NULL, *dev, NULL, name);
        if (IS_ERR(pdev)) {
                printk(KERN_ERR"unable create device \n");
		goto error_reg; 		
        }

	dev_count++;

return pdev;

error_reg:
	return NULL;
}


int remove_device(dev_t *dev, struct cdev *cdev)
{
	if (debug_class) {
		if (dev_count) {
			cdev_del(cdev);
        		device_destroy(debug_class, *dev);
        		unregister_chrdev_region(*dev, MINOR_COUNT);
			dev_count--;
		}
	}

	if(!dev_count) {
		if(debug_class) 
	       	cln_debug_class();
		else {
			printk(KERN_ALERT "Class not present \n");
			return 0;
		}		
	}

	return 0;
}
