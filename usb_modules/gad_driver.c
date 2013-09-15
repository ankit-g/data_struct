#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include "ioctl.h"


#include "epautoconf.c"
#include "config.c"

/* Big enough to hold our biggest descriptor */
#define EP0_BUFSIZE     512
#define EP0_DELAYED     512*0xFF
#define BLK_BUFSIZE     4096


static struct usb_device_descriptor
device_desc = {
        .bLength 		= sizeof device_desc,
        .bDescriptorType	= USB_DT_DEVICE,

        .bcdUSB 		= cpu_to_le16(0x0200),
        .bDeviceClass 		= USB_CLASS_VENDOR_SPEC,

        /* The next three values can be overridden by module parameters */
        .idVendor 		= cpu_to_le16(0x0525),
        .idProduct 		= cpu_to_le16(0xA4A0),
        .bcdDevice 		= cpu_to_le16(0xffff),

        .bNumConfigurations 	=   1,
};

/*Initially config_descriptor has 0 interfaces so setup request can complete easily*/
static struct usb_config_descriptor
config_desc = {
	.bLength 		= sizeof config_desc,
	.bDescriptorType 	= USB_DT_CONFIG,

	/* wTotalLength computed later */
	.bNumInterfaces 	= 1,
	.bConfigurationValue 	= 66,
	.bmAttributes 		= USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
	.bMaxPower 		= CONFIG_USB_GADGET_VBUS_DRAW / 2,
};

static struct usb_interface_descriptor intf_desc  = {
        .bLength                = USB_DT_INTERFACE_SIZE,
        .bDescriptorType        = USB_DT_INTERFACE,
        .bInterfaceNumber       = 0,
        .bAlternateSetting      = 0,
        .bNumEndpoints          = 2,
        .bInterfaceClass        = 0xff,
        .bInterfaceSubClass     = 0xff,
        .bInterfaceProtocol     = 0x00,
        .iInterface             = 0,
};

static struct usb_endpoint_descriptor
gad_fs_bulk_in_desc = {
        .bLength =              USB_DT_ENDPOINT_SIZE,
        .bDescriptorType =      USB_DT_ENDPOINT,

        .bEndpointAddress =     USB_DIR_IN,
        .bmAttributes =         USB_ENDPOINT_XFER_BULK,
        /* wMaxPacketSize set by autoconfiguration */
};

static struct usb_endpoint_descriptor
gad_fs_bulk_out_desc = {
        .bLength =              USB_DT_ENDPOINT_SIZE,
        .bDescriptorType =      USB_DT_ENDPOINT,

        .bEndpointAddress =     USB_DIR_OUT,
        .bmAttributes =         USB_ENDPOINT_XFER_BULK,
        /* wMaxPacketSize set by autoconfiguration */
};

static struct usb_endpoint_descriptor
gad_hs_bulk_in_desc = {
        .bLength =              USB_DT_ENDPOINT_SIZE,
        .bDescriptorType =      USB_DT_ENDPOINT,

        /* bEndpointAddress copied from fs_bulk_in_desc during fsg_bind() */
        .bmAttributes =         USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize =       cpu_to_le16(512),
};

static struct usb_endpoint_descriptor
gad_hs_bulk_out_desc = {
        .bLength =              USB_DT_ENDPOINT_SIZE,
        .bDescriptorType =      USB_DT_ENDPOINT,

        /* bEndpointAddress copied from fs_bulk_out_desc during fsg_bind() */
        .bmAttributes =         USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize =       cpu_to_le16(512),
        .bInterval =            1,      /* NAK every 1 uframe */
};



static struct usb_descriptor_header *gadget_fs_descs[] = {
        (struct usb_descriptor_header *) &intf_desc,
	(struct usb_descriptor_header *) &gad_fs_bulk_in_desc, 
	(struct usb_descriptor_header *) &gad_fs_bulk_out_desc, 
        NULL,
};

static struct usb_descriptor_header *gadget_hs_descs[] = {
        (struct usb_descriptor_header *) &intf_desc,
	(struct usb_descriptor_header *) &gad_hs_bulk_in_desc, 
	(struct usb_descriptor_header *) &gad_hs_bulk_out_desc, 
        NULL,
};

/*
 * Config descriptors must agree with the code that sets configurations
 * and with code managing interfaces and their altsettings.  They must
 * also handle different speeds and other-speed requests.
 */
static int populate_config_buf(struct usb_gadget *gadget,
                u8 *buf, u8 type, unsigned index)
{
        enum usb_device_speed                   speed = gadget->speed;
        int                                     len;
        const struct usb_descriptor_header      **function;

        if (index > 0)
                return -EINVAL;

        if (gadget_is_dualspeed(gadget) && type == USB_DT_OTHER_SPEED_CONFIG)
                speed = (USB_SPEED_FULL + USB_SPEED_HIGH) - speed;
        function = gadget_is_dualspeed(gadget) && speed == USB_SPEED_HIGH
                ? (const struct usb_descriptor_header **)gadget_hs_descs
                : (const struct usb_descriptor_header **)gadget_fs_descs;

        /* for now, don't advertise srp-only devices */
        if (!gadget_is_otg(gadget))
                function++;

        len = usb_gadget_config_buf(&config_desc, buf, EP0_BUFSIZE, function);
        ((struct usb_config_descriptor *) buf)->bDescriptorType = type;
        return len;
}




struct usbg_dev {
        struct usb_gadget       *gadget;        /* Copy of cdev->gadget */
      
	struct usb_ep           *ep0;           /* Copy of gadget->ep0 */
        struct usb_request      *ep0req;        /* Copy of cdev->req */
 
        struct usb_ep           *bulk_in;
        struct usb_request      *inreq;        /* Copy of cdev->req */

        struct usb_ep           *bulk_out;
        struct usb_request      *outreq;        /* Copy of cdev->req */
};     

static struct usbg_dev *_usbg_dev;
static struct class *usbg_class;
static struct cdev cdev;
static dev_t dev;
static struct device *pdev;

static int usbg_setup(struct usb_gadget * , const struct usb_ctrlrequest * );
static void usbg_unbind(struct usb_gadget * );
static void usbg_disconnect(struct usb_gadget * );

ssize_t usbg_read (struct file *, char __user *, size_t, loff_t *);
static long usbg_ioctl(struct file *, unsigned int, unsigned long);

static struct file_operations fops = {
	.owner          = THIS_MODULE,
	.read 		= usbg_read,
	.unlocked_ioctl	= usbg_ioctl
};

static long usbg_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;

	switch (cmd) {
	case USBDSBL:
		printk(KERN_DEBUG"usb disable received\n");
		ret = usb_gadget_disconnect(_usbg_dev->gadget);
		if (ret) {
			printk(KERN_ERR"not supported\n");
			goto fail_cond;
		}
		break;
	case USBENBL:
		printk(KERN_DEBUG"usb enable received\n");
		ret = usb_gadget_connect(_usbg_dev->gadget);
		if (ret) {
			printk(KERN_ERR"not supported\n");
			goto fail_cond;
		}

		break;	
	}
	
fail_cond:
	return ret;
}	


ssize_t usbg_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	printk(KERN_DEBUG"%s\n", __func__);
	
	return size;
}

static struct usb_gadget_driver usbg_driver = {
        .function 	= "usb gadget driver",     
        .speed 		= USB_SPEED_HIGH,
        .unbind 	= usbg_unbind,  
        .setup 		= usbg_setup,    
        .disconnect 	= usbg_disconnect,  

        .driver 	= {             
                .owner 	= THIS_MODULE,
                .name 	= "usb gadget driver"  
        },
};

static void ep0_complete(struct usb_ep *ep, struct usb_request *req)
{ 
	 printk(KERN_DEBUG"%s --> %d, %u/%u\n", __func__,
                                req->status, req->actual, req->length);
}

static void usbg_disconnect(struct usb_gadget *gadget)
{
	printk(KERN_DEBUG"%s\n", __func__);
}

static void usbg_unbind(struct usb_gadget * gadget)
{
	printk(KERN_DEBUG"%s\n", __func__);
}

static inline struct usb_endpoint_descriptor *choose_ep_desc(struct usb_endpoint_descriptor *hs, 
struct usb_endpoint_descriptor *fs)
{
	return ((gadget_is_dualspeed(_usbg_dev->gadget) && (_usbg_dev->gadget->speed == USB_SPEED_HIGH) ? hs : fs));
}


static int gad_enable_eps(void)
{
	struct usb_ep *ep;
	struct usb_request *req;
	int err = 0;
	
	ep = _usbg_dev->bulk_in;
	ep->desc = choose_ep_desc(&gad_hs_bulk_in_desc, &gad_fs_bulk_in_desc);
	err = usb_ep_enable(ep);
        if (err) {
                printk(KERN_ERR"can't start %s: %d\n", _usbg_dev->bulk_in->name, err);
                goto fail;
        }
	
	ep = _usbg_dev->bulk_out;
	ep->desc = choose_ep_desc(&gad_hs_bulk_out_desc, &gad_fs_bulk_out_desc);
        err = usb_ep_enable(ep);
        if (err) {
                printk(KERN_ERR"can't start %s: %d\n", _usbg_dev->bulk_in->name, err);
                goto fail;
        }

	// Initialize the request queue for bulk endpoints.
	_usbg_dev->inreq = req = usb_ep_alloc_request(_usbg_dev->bulk_in, GFP_ATOMIC);

        if (!req) {
                err = -ENOMEM;
                printk(KERN_ERR"usb_ep_alloc_request failed\n");
                goto fail;
        }

        req->buf = kmalloc(BLK_BUFSIZE, GFP_ATOMIC);
        if (!req->buf) {
                err = -ENOMEM;
                printk(KERN_ERR"unable to allocate req->buf\n");
                goto fail;
        }

        req->complete = ep0_complete;

        _usbg_dev->outreq = req = usb_ep_alloc_request(_usbg_dev->bulk_out, GFP_ATOMIC);

        if (!req) {
                err = -ENOMEM;
                printk(KERN_ERR"usb_ep_alloc_request failed\n");
                goto fail;
        }

        req->buf = kmalloc(BLK_BUFSIZE, GFP_ATOMIC);
        if (!req->buf) {
                err = -ENOMEM;
                printk(KERN_ERR"unable to allocate req->buf\n");
                goto fail;
        }

        req->complete = ep0_complete;





fail:
	return err;
}

static int usbg_setup(struct usb_gadget * gadget, const struct usb_ctrlrequest * ctrlreq)
{
	int 			rc = 0;
	int			value;	
	int			dir = (ctrlreq->bRequestType & 0x80) ? 1 : 0;
	u16			w_index = le16_to_cpu(ctrlreq->wIndex);
        u16                     w_value = le16_to_cpu(ctrlreq->wValue);
        u16                     w_length = le16_to_cpu(ctrlreq->wLength);

	struct usb_request	*req = _usbg_dev->ep0req;

	printk(KERN_DEBUG"\n%s\n", __func__);
	printk(KERN_DEBUG"bRequestType %x bRequest %x wValue %x wIndex %d wLength %d\n"
		, ctrlreq->bRequestType, ctrlreq->bRequest, w_value, w_index, w_length);

	if(dir) {
		printk(KERN_DEBUG"req dir = IN\n");

		switch (ctrlreq->bRequest) {
		case USB_REQ_GET_DESCRIPTOR: 
			printk(KERN_DEBUG"GET_DES \n");
		  	switch (w_value >> 8) { 

			case USB_DT_DEVICE:
				printk(KERN_DEBUG"Get device descriptor\n");
				device_desc.bMaxPacketSize0 = _usbg_dev->ep0->maxpacket;
				value = min(w_length, (u16)sizeof(struct usb_device_descriptor));
				req->length = value;
				memcpy(req->buf, &device_desc, value);
				value = usb_ep_queue(_usbg_dev->ep0, req, GFP_ATOMIC);
				if (value < 0) {
					req->status = 0;
					ep0_complete(_usbg_dev->ep0, req);			
				}
				break;

			case USB_DT_CONFIG:
				printk(KERN_DEBUG"Get config descriptor %d \n", w_length);
				value = populate_config_buf(gadget,
                                        req->buf,
                                        w_value >> 8,
                                        w_value & 0xff);
				value = min(w_length, (u16)value);
				req->length = value;
				value = usb_ep_queue(_usbg_dev->ep0, req, GFP_ATOMIC);
				if (value < 0) {
					req->status = 0;
					ep0_complete(_usbg_dev->ep0, req);			
				}
				break;
					
			}
			break;							
		}

	}
	else {
		printk(KERN_DEBUG"OUT REQ SENT\n");	
		switch (ctrlreq->bRequest) {
		case USB_REQ_SET_CONFIGURATION:
			printk(KERN_DEBUG"set config %d\n", w_value);		
			// Enable Endpoints and allocate request
			gad_enable_eps();

			// Wait for ZLP from host for completion 
			// of setting of configuration.
			req->length = EP0_DELAYED;
			value = usb_ep_queue(_usbg_dev->ep0, req, GFP_ATOMIC);
                                if (value < 0) {
                                        req->status = 0;
                                        ep0_complete(_usbg_dev->ep0, req);
                                }
                                break;
	
			break;
		case USB_REQ_SET_INTERFACE:
			printk(KERN_DEBUG"set interface %d\n", w_value);			
			break;
		}
	}
	return rc;
}

static int __init usbg_bind(struct usb_gadget *gadget)
{
	int rc = 0;
	struct usb_request *req;
	struct usb_ep *ep;

	printk(KERN_DEBUG"%s\n", __func__);
	
	_usbg_dev = kmalloc(sizeof(struct usbg_dev), GFP_KERNEL);

        if(!_usbg_dev) {
                rc = -ENOMEM;
                printk(KERN_ERR"kmalloc failed\n");
                goto err_release;
        }

        /*
         * usb_ep_autoconfig_reset - reset endpoint autoconfig state1
         */

        usb_ep_autoconfig_reset(gadget);

        _usbg_dev->gadget = gadget;
        _usbg_dev->ep0    = gadget->ep0;
        _usbg_dev->ep0->driver_data = _usbg_dev;

	_usbg_dev->ep0req = req = usb_ep_alloc_request(_usbg_dev->ep0, GFP_KERNEL);

	if (!req) {
		rc = -ENOMEM;
                printk(KERN_ERR"usb_ep_alloc_request failed\n");
                goto err_release;
	}

	req->buf = kmalloc(EP0_BUFSIZE, GFP_KERNEL);
        if (!req->buf) {
                rc = -ENOMEM;
                printk(KERN_ERR"unable to allocate req->buf\n");
                goto err_release;
	}
	
	req->complete = ep0_complete;

	// Configuring remaining of the endpoints

	ep = usb_ep_autoconfig(gadget, &gad_fs_bulk_in_desc);
        if (!ep)
                goto autoconf_fail;
        ep->driver_data = _usbg_dev;          // claim the endpoint
        _usbg_dev->bulk_in = ep;
	
	ep = usb_ep_autoconfig(gadget, &gad_fs_bulk_out_desc);
        if (!ep)
                goto autoconf_fail;
        ep->driver_data = _usbg_dev;          // claim the endpoint
        _usbg_dev->bulk_out = ep;

	/* Any highspeed device must support full speed as well.
	 * The device is initialy always detected as full speed. 
	 * Then after this the device further communicates as a
	 * High speed device if the host is high speed.
	 */

	if (gadget_is_dualspeed(gadget)) {

                /* Assume endpoint addresses are the same for both speeds */
                gad_hs_bulk_in_desc.bEndpointAddress =
                        gad_fs_bulk_in_desc.bEndpointAddress;
                gad_hs_bulk_out_desc.bEndpointAddress =
                        gad_fs_bulk_out_desc.bEndpointAddress;
        }

	return 0;

autoconf_fail:
        printk(KERN_ERR"unable to autoconfigure all endpoints\n");
        rc = -ENOTSUPP;
	
err_release:

        return rc;
}

static int __init gad_init(void)
{
	int ret;

	printk(KERN_DEBUG"%s\n", __func__);

	ret=alloc_chrdev_region(&dev,0,1,"usbg_driver");
	if (ret!=0) {
		printk(KERN_ALERT "alloc_chrdev failed!!!\n");
		return ret; 
	}   

	cdev.owner = THIS_MODULE;
	cdev_init(&cdev,&fops);

	ret = cdev_add(&cdev,dev,1);
	if (ret) {
		printk(KERN_ALERT "cdev_add failed!!!\n");
		return ret; 
	}

	
	usbg_class = class_create(THIS_MODULE, "usbg_device");
	if (IS_ERR(usbg_class))	{
		printk(KERN_ERR"unable to create class\n");
		return PTR_ERR(usbg_class);
	}

	pdev = device_create(usbg_class, NULL, dev, NULL, "usb_gad_node");
	if (IS_ERR(pdev)) {
		 printk(KERN_ERR"unable create device node\n");
		 return PTR_ERR(pdev);
	}	
		
	return usb_gadget_probe_driver(&usbg_driver, usbg_bind);
}

static void __exit gad_exit(void)
{
	printk(KERN_DEBUG"%s\n", __func__);

	cdev_del(&cdev);
	device_destroy(usbg_class, dev);
	unregister_chrdev_region(dev,1);

	class_destroy(usbg_class);
	usbg_class = NULL;	
	kfree(_usbg_dev->ep0req->buf);
	kfree(_usbg_dev);
	usb_gadget_unregister_driver(&usbg_driver);
}

module_init(gad_init);
module_exit(gad_exit);

MODULE_DESCRIPTION("Simple Gadget Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ankit Gupta <ankitgupta.work@gmail.com>");
