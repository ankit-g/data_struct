#define DEBUG
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#ifdef DEBUG
#define debug(fmt, args...) do { /*printk(KERN_DEBUG"%s(): ", __func__);*/printk(KERN_DEBUG fmt, ##args); } while (0)
#else
#define debug(fmt, args...) do { } while(0)
#endif

extern int create_char_debug_class(void);
extern void cln_debug_class(void);
extern struct device *crt_debug_device(const struct file_operations *, struct cdev *, dev_t *, const char *);
extern int remove_device(dev_t *, struct cdev *);
