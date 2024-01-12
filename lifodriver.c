#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>    //kmalloc()
#include <linux/uaccess.h> //copy_to/from_user()

#define WR_VALUE _IOW('a', 'a', int32_t *)
#define RD_VALUE _IOR('a', 'b', int32_t *)
#define PUSH _IOW('a', 'c', int32_t *)
#define POP _IOR('a', 'd', int32_t *)
#define BUFF_SIZE 20

static struct stack {
  char buff[BUFF_SIZE];
  int size;
};

struct stack driver_stack;

char value[BUFF_SIZE];

dev_t dev = 0;
static struct class *dev_class;
static struct cdev lifo_cdev;

static int __init lifo_driver_init(void);
static void __exit lifo_driver_exit(void);
static int lifo_open(struct inode *inode, struct file *file);
static int lifo_release(struct inode *inode, struct file *file);
static ssize_t lifo_read(struct file *filp, char __user *buf, size_t len,
                         loff_t *off);
static ssize_t lifo_write(struct file *filp, const char *buf, size_t len,
                          loff_t *off);
static long lifo_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = lifo_read,
    .write = lifo_write,
    .open = lifo_open,
    .unlocked_ioctl = lifo_ioctl,
    .release = lifo_release,
};

static int lifo_open(struct inode *inode, struct file *file) {
  pr_info("Device File Opened...!!!\n");
  return 0;
}

static int lifo_release(struct inode *inode, struct file *file) {
  pr_info("Device File Closed...!!!\n");
  return 0;
}

static ssize_t lifo_read(struct file *filp, char __user *buf, size_t len,
                         loff_t *off) {
  pr_info("Read Function\n");
  return 0;
}

static ssize_t lifo_write(struct file *filp, const char __user *buf, size_t len,
                          loff_t *off) {
  pr_info("Write function\n");
  return len;
}

static long lifo_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
  switch (cmd) {
  case WR_VALUE:
    if (copy_from_user(value, (char *)arg, sizeof(value))) {
      pr_err("Data Write : Err!\n");
    }
    pr_info("Value = %d\n", value);
    break;
  case RD_VALUE:
    char value[1];
    value[0] = driver_stack.buff[driver_stack.size - 1];
    printk(KERN_INFO "IOCTL after: %c\n", value[0]);
    copy_to_user(arg, value, 1);
    printk(KERN_INFO "IOCTL after: %c\n", value[0]);
    driver_stack.size -= 1;
    break;
  case PUSH:
    char temp[20];
    copy_from_user(temp, arg, 1);
    int i = 1;
    while (temp[i - 1] != NULL) {
      copy_from_user(temp + i, arg + i, 1);
      i++;
    }
    memcpy(driver_stack.buff + driver_stack.size, temp, strlen(temp));
    driver_stack.size += strlen(temp);
    printk(KERN_ALERT "IOCTL: %s", driver_stack.buff);
    break;
  default:
    pr_info("Default\n");
    break;
  }
  return 0;
}

static int __init lifo_driver_init(void) {
  driver_stack.buff;
  driver_stack.size = 0;
  /*Allocating Major number*/
  if ((alloc_chrdev_region(&dev, 0, 1, "lifo_Dev")) < 0) {
    pr_err("Cannot allocate major number\n");
    return -1;
  }
  pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

  /*Creating cdev structure*/
  cdev_init(&lifo_cdev, &fops);

  /*Adding character device to the system*/
  if ((cdev_add(&lifo_cdev, dev, 1)) < 0) {
    pr_err("Cannot add the device to the system\n");
    goto r_class;
  }

  /*Creating struct class*/
  if (IS_ERR(dev_class = class_create("lifo_class"))) {
    pr_err("Cannot create the struct class\n");
    goto r_class;
  }

  /*Creating device*/
  if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "lifo_device"))) {
    pr_err("Cannot create the Device 1\n");
    goto r_device;
  }
  pr_info("Device Driver Insert...Done!!!\n");
  return 0;

r_device:
  class_destroy(dev_class);
r_class:
  unregister_chrdev_region(dev, 1);
  return -1;
}

static void __exit lifo_driver_exit(void) {
  device_destroy(dev_class, dev);
  class_destroy(dev_class);
  cdev_del(&lifo_cdev);
  unregister_chrdev_region(dev, 1);
  pr_info("Device Driver Remove...Done!!!\n");
}

module_init(lifo_driver_init);
module_exit(lifo_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Spaghetti Sensei");
MODULE_DESCRIPTION("Simple Linux device driver (IOCTL)");
MODULE_VERSION("0.1");