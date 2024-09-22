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

#include "gpio.h"
#include "asm/uaccess.h"
#include "linux/export.h"
#include "linux/printk.h"
#include  "linux/poll.h"
#include "linux/wait.h"
#define LED_NUM 2
// first define device main num
int major = 0;
struct gpio_op* p_led_opr;
struct class* LED_class;

static DECLARE_WAIT_QUEUE_HEAD(gpio_key_wait);

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
    printk("read device\n");
    result = p_led_opr -> read(minor);
    if (result == 0)
    {   
        wait_event_interruptible(gpio_key_wait, p_led_opr -> read(minor));
        printk("wait success\n");
        result = 1;
    }
    else {
        printk("wait failed\n");
    }
    printk("wait exit\n");
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

unsigned int my_device_poll(struct file *file, poll_table *wait)
{
    unsigned int mask = 0;
    struct inode *inode = file_inode(file);
    int minor = iminor(inode);
    printk("poll device\n");
    /* 将等待队列添加到poll_table */
    poll_wait(file, &gpio_key_wait, wait);
    printk("poll device 2\n");
    /* 检查设备状态，设置相应的事件掩码 */
    if ((p_led_opr -> read(minor)) == 1)
        mask |= POLLIN | POLLRDNORM;  // 可读事件

    return mask;
}


// define device file operation
struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .read = led_read,
    .write = led_write,
    .open = led_open,
    .release = led_release,
    .poll = my_device_poll,
};

static int __init led_init(void)
{
    int err;
    major = register_chrdev(major,"jsdfhasuh",&led_fops);
    LED_class = class_create(THIS_MODULE, "jsdfhasuh_class");
    
    err = PTR_ERR(LED_class);
    if (IS_ERR(LED_class)) {
		printk("error PTR_ERR %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "jsdfhasuh");
		return -1;
	}
    printk("success load LED drive\n");
    return 0;
}


void create_device_point(int num)
{
    device_create(LED_class, NULL, MKDEV(major, num), NULL, "jsdfhasuh%d", num);
}

void destroy_device(int num)
{
    device_destroy(LED_class, MKDEV(major, num));
}

void give_p_led_opr (struct gpio_op* p)
{
    p_led_opr = p;
}

wait_queue_head_t * get_wait_queue(void)
{
    return &gpio_key_wait;
}



EXPORT_SYMBOL(create_device_point);
EXPORT_SYMBOL(destroy_device);
EXPORT_SYMBOL(give_p_led_opr);
EXPORT_SYMBOL(get_wait_queue);


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