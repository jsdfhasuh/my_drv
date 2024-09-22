#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>

#include "asm/uaccess.h"
#include "linux/export.h"
#include "linux/printk.h"
#include "jsdfhasuh_cdev.h"


// first define device main num
int major = 0;
struct gpio_op* gpio_opr;
struct gpio_resource* gpio_resources;
struct class* GPIO_class = NULL;



// define device open function
int gpio_open (struct inode * node, struct file * file)
{   
    char status = 0;
    int second = iminor(node);
    gpio_opr -> init(second,status);
    return 0;
}


// define device release function
int gpio_release (struct inode * node, struct file * file)
{
    printk("release device");
    return 0;
}


// define device read function
ssize_t gpio_read (struct file* file, char __user* user_buff, size_t sieze, loff_t * offset)
{
    printk("begin read\n");
    int err;
    int i;
    int result;
	struct inode *inode = file_inode(file);
    int minor = iminor(inode);
    char * data_ptr = NULL;
    unsigned long  data_len_ptr;
    while (1)
    {
        result = gpio_opr->read(minor,&data_ptr,&data_len_ptr);
        if (result == 1)
            break;
        else
            printk("重新执行");
    }
    for (i =0;i<5;i++)
        printk("data %d is %d",i,data_ptr[i]);
    printk("read device result is %d\n", result);
    err = copy_to_user(user_buff, data_ptr, data_len_ptr);
    if (err == 0)
        return 0; // 返回0表示读取成功
    else
        return -EFAULT; // 复制失败
}

// define device write function
ssize_t gpio_write (struct file* file, const char __user* user_buff, size_t size , loff_t * offset)
{
	int kernel_buffer;
    int err;
    int result;
    struct inode * inode = file_inode(file);
    int minor = iminor(inode);
    printk("write device\n");
    if (!user_buff) 
    {
    printk("user_buff is NULL\n");
    return -EFAULT;
    }
    err = copy_from_user(&kernel_buffer, user_buff, 2);
    if (err == 0)
        printk("copy from user success\n");
    else
        printk("copy from user failed\n");
    printk("user data is %d\n", kernel_buffer);
    result = gpio_opr -> write(minor, kernel_buffer);
    return 0;
}


// define device file operation
struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
    .read = gpio_read,
    .write = gpio_write,
    .open = gpio_open,
    .release = gpio_release,
};

static int __init gpio_init(void)
{
    printk("success load gpio mudles\n");
    return 0;
}

int jsdfhasuh_register_chrdev(void)
{
    int err;
    char *cdev_name= gpio_opr ->dev_name;
    major = register_chrdev(major,cdev_name,&gpio_fops);
    // class_exist = class_find("jsdfhasuh_class"); // Removed as it is not a valid function
    if (GPIO_class == NULL)
    {
        GPIO_class = class_create(THIS_MODULE, "jsdfhasuh_class");
        printk("success create GPIO class %s\n",cdev_name);
        err = PTR_ERR(GPIO_class);
        if (IS_ERR(GPIO_class)) {
            printk("error PTR_ERR %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
            unregister_chrdev(major, cdev_name);
            return -1;
        }
    }
    printk("success create GPIO class");
    return 0;
}


int jsdfhasuh_unregister_chrdev(void)
{
    char *cdev_name= gpio_opr ->dev_name;
    major = 0;
    // class_exist = class_find("jsdfhasuh_class"); // Removed as it is not a valid function
    if (GPIO_class != NULL)
    {
        class_destroy(GPIO_class);
        GPIO_class = NULL;
        unregister_chrdev(major,cdev_name);
        printk("success destroy GPIO class %s\n",cdev_name);
    }
    return 0;
}


void create_device_point(int num)
{
    if (GPIO_class != NULL)
    {
        device_create(GPIO_class, NULL, MKDEV(major, num), NULL, "jsdfhasuh_%s_%d", gpio_opr ->dev_name,num);
        printk("jsdfhasuh%s_%d has create\n", gpio_opr ->dev_name,num);
    }
}

void destroy_device(int num)
{
    if (GPIO_class != NULL)
    {
        device_destroy(GPIO_class, MKDEV(major, num));
        printk("jsdfhasuh%d,device delete\n",num);
    }
}

void give_gpio_opr (struct gpio_op* p, gpio_resource * gpio_resource_ptr)
{
    printk("pass gpio resource and gpio operation");
    gpio_resources = gpio_resource_ptr;
    gpio_opr = p;
}

EXPORT_SYMBOL(create_device_point);
EXPORT_SYMBOL(destroy_device);
EXPORT_SYMBOL(give_gpio_opr);
EXPORT_SYMBOL(jsdfhasuh_register_chrdev);
EXPORT_SYMBOL(jsdfhasuh_unregister_chrdev);


static void __exit gpio_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
    if (GPIO_class != NULL)
    {
        printk("delete Class");
        class_destroy(GPIO_class);
	    unregister_chrdev(major, gpio_opr ->dev_name);
    }
}


module_init(gpio_init);
module_exit(gpio_exit);
MODULE_AUTHOR("jsdfhasuh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GPIO Base module");