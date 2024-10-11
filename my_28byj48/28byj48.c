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
#include "linux/string.h"
#include "linux/timer.h"
#include "linux/irqflags.h"
#include "linux/jiffies.h"
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/delay.h>
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
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/timer.h>
#include "linux/printk.h"
#include "linux/wait.h"
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

// first define device main num

int major = 0;
int gpio_num;
struct timer_list task_timer;
char *cdev_name= "28byj48";
int run_index = 0;
int last_run_index = 0;
int run_times=0;
int off_index = 0;
int status = 0;
int k=0;

struct class* GPIO_class = NULL; 
typedef struct gpio_resource {
    int index;
    struct gpio_desc *gpio_info;
    int gpio;
} gpio_resource;

gpio_resource * gpio_resources;

// define device open function
int bj_open (struct inode * node, struct file * file)
{ 

    printk("init 28byj48 device");
    return 0;
}


// define device release function
int bj_release (struct inode * node, struct file * file)
{
    printk("release 28byj48 device");
    return 0;
}

static void run(unsigned long data)
{
    if (k == 4096)
    {
        printk("1 cycle");
        k = 0;
    }
    k++;
    if (!status)
        return;
    if(run_times == 2)
    {
        gpiod_set_value(gpio_resources[off_index].gpio_info, 0);
        run_times = 1;
        mod_timer(&(task_timer),jiffies);
        return;
    }
    gpiod_set_value(gpio_resources[run_index].gpio_info, 1);
    if (run_times == 1)
    {
        off_index = last_run_index;
    }
    last_run_index = run_index;
    run_index ++;
    run_times ++;
    k++;
    if (run_index == 4)
        run_index = 0;
    mod_timer(&(task_timer),jiffies);
}

// define device write function
ssize_t bj_write (struct file* file, const char __user* user_buff, size_t size , loff_t * offset)
{
    int kernel_buffer[4];
    int error;
    int i;
    error = copy_from_user(kernel_buffer, user_buff, sizeof(kernel_buffer));
    if (kernel_buffer[0] == 1)
        {
            printk("on");
            status = 1;
            printk("speed is %d",kernel_buffer[1]);
            mod_timer(&(task_timer),jiffies);
        }
    else
        {
            printk("off");
            status = 0;
            del_timer(&task_timer);
            for (i = 0;i<4;i++)
                gpiod_set_value(gpio_resources[i].gpio_info, 0);
        }
    printk("change device status\n");
    return 0;
}


struct file_operations bj_fops = {
    .owner = THIS_MODULE,
    .write = bj_write,
    .open = bj_open,
    .release = bj_release,
};

int bj_probe(struct platform_device * pdev)
{
    int i = 0;
    int error = 0;
    enum gpiod_flags flags = GPIOD_OUT_LOW;
    // file operations register
    major = register_chrdev(major,cdev_name,&bj_fops);
    GPIO_class = class_create(THIS_MODULE, "28byj48");
    printk("success create GPIO class %s\n",cdev_name);
    error = PTR_ERR(GPIO_class);
    if (IS_ERR(GPIO_class)) {
        printk("error PTR_ERR %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        unregister_chrdev(major, cdev_name);
        return -1;
    }
    // gpio info register
    gpio_num = gpiod_count(&pdev->dev, "28byj48");
    printk("gpio_num is %d\n",gpio_num);
    gpio_resources = (gpio_resource *)kmalloc(gpio_num*sizeof(gpio_resource),GFP_ATOMIC);
    for (i = 0;i < gpio_num;i++)
    {
        printk("gpio_index is %d\n",i);
        gpio_resources[i].index = i;
        gpio_resources[i].gpio_info = gpiod_get_index(&pdev->dev, "28byj48", i,flags);
        if (IS_ERR(gpio_resources[i].gpio_info)) 
        {
            dev_err(&pdev->dev, "Failed to get GPIO descriptor\n");
            return PTR_ERR(gpio_resources[i].gpio_info);
        }
    }
    printk("get gpio_info success\n");
    setup_timer(&task_timer, run, (unsigned long)(NULL));
    device_create(GPIO_class, NULL, MKDEV(major, 0), NULL, "jsdfhasuh_%s_%d", "28byj48",0);
    printk("jsdfhasuh_%s_%d has create\n", "28byj48",0);
    return  0;
}

int bj_remove(struct platform_device * pdev)
{
    int i;
    for (i = 0; i < gpio_num; i++) 
    {
        gpiod_put(gpio_resources[i].gpio_info);
    }
    device_destroy(GPIO_class, MKDEV(major, 0));
    printk("jsdfhasuh%d,device delete\n",0);
    major = 0;
    class_destroy(GPIO_class);
    GPIO_class = NULL;
    unregister_chrdev(major,cdev_name);
    printk("success destroy GPIO class %s\n",cdev_name);
    kfree(gpio_resources);
    printk("28byj48 device removed\n");
    return 0;
}

static const struct of_device_id bj_table[] = {
	{ .compatible = "28byj48"},
};

static struct platform_driver bj_driver = {
    .probe      = bj_probe,
    .remove     = bj_remove,
    .driver     = {
    .name   = "jsdfhasuh_28byj48",
    .of_match_table = bj_table,
    },
};

static int __init bj_drv_init(void)
{
    int error;
    error = platform_driver_register(&bj_driver); 
    printk("success load 28byj48\n");
    return 0;
}

static void __exit bj_drv_exit(void)
{
    platform_driver_unregister(&bj_driver);
}

module_init(bj_drv_init);
module_exit(bj_drv_exit);
MODULE_AUTHOR("jsdfhasuh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("28byj48");