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

#include "LED.h"
#include "asm/uaccess.h"
#include "linux/export.h"
#include "linux/printk.h"
#define  LED_NUM 2


// first define device main num
int major = 0;
struct led_op* p_led_opr;
struct class* LED_class;



// define device open function
int led_open (struct inode * node, struct file * file)
{   
    char status = 0;
    int second = iminor(node);
    p_led_opr ->init(second,status);
    return 0;
}


// define device release function
int led_release (struct inode * node, struct file * file)
{
    printk("release device");
    return 0;
}
// define device read function
ssize_t led_read (struct file* file, char __user* user_buff, size_t sieze, loff_t * offset)
{
    int err;
    int result;
	struct inode *inode = file_inode(file);
    int minor = iminor(inode);
    result = p_led_opr->read(minor);
    printk("read device result is %d\n", result);
    err = copy_to_user(user_buff, &result, sizeof(int));
    if (err == 0)
        return 0; // 返回0表示读取成功
    else
        return -EFAULT; // 复制失败
}

// define device write function
ssize_t led_write (struct file* file, const char __user* user_buff, size_t size , loff_t * offset)
{
	char kernel_buffer;
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
    result = p_led_opr -> ctl(minor, kernel_buffer);
    return 0;
}


// define device file operation
struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .read = led_read,
    .write = led_write,
    .open = led_open,
    .release = led_release,
};

static int __init led_init(void)
{
    int err;
    int i;
    major = register_chrdev(major,"jsdfhasuh",&led_fops);
    printk("this is my led 2");
    LED_class = class_create(THIS_MODULE, "jsdfhasuh_class");
    
    err = PTR_ERR(LED_class);
    if (IS_ERR(LED_class)) {
		printk("error PTR_ERR %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "jsdfhasuh");
		return -1;
	}

    for (i = 0; i < LED_NUM; i++)
        device_create(LED_class, NULL, MKDEV(major, i), NULL, "jsdfhasuh%d", i); /* /dev/100ask_led0,1,... */
    p_led_opr = get_board_led_op();
    return 0;
}


static void __exit led_exit(void)
{
	int i;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	for (i = 0; i < LED_NUM; i++)
		device_destroy(LED_class, MKDEV(major, i)); /* /dev/100ask_led0,1,... */

	device_destroy(LED_class, MKDEV(major, 0));
	class_destroy(LED_class);
	unregister_chrdev(major, "jsdfhasuh");
}


module_init(led_init);
module_exit(led_exit);
MODULE_AUTHOR("jsdfhasuh");
MODULE_LICENSE("GPL");