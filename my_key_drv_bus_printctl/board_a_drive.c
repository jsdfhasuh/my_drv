#include <linux/module.h>
#include <linux/gpio/consumer.h>
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
#include <linux/slab.h>
#include "asm/uaccess.h"
#include "linux/export.h"
#include "linux/gpio.h"
#include "linux/of.h"
#include "linux/printk.h"
#include "gpio.h"
#include <asm/io.h>
#include <linux/platform_device.h>


int io_status = 0; // 0 为输入 1 为输出
int gpio_num = 0;
struct device_info{
    int gpio_num;
    int gpio_status;    
    struct gpio_desc *gpio_info;
};

static struct device_info *device_infos;

struct gpio_desc *gpio_info;



int board_led_init(int which,char status)
{
    
    if (status == 1)
    {   
        printk("enable output\n");
        gpiod_direction_output(device_infos[which].gpio_info, 1);
    }
    else
    {
        printk("enable input\n");
        gpiod_direction_input(device_infos[which].gpio_info);
    }
    return 0;
}

int board_led_control(int which,char status)
{

    if (status == 1)
    {
        gpiod_set_value(device_infos[which].gpio_info, 1);
    }
    else
    {
        gpiod_set_value(device_infos[which].gpio_info, 0);
    }
    return 0;
}

int board_led_read(int which)
{
    return gpiod_get_value(device_infos[which].gpio_info);
}

static struct gpio_op board_demo_led_opr = {
	.init = board_led_init,
	.ctl  = board_led_control,
    .read = board_led_read,
};

int get_board_led_op(void)
{
    give_p_led_opr(&board_demo_led_opr);
    return 0;
}

int led_probe (struct platform_device * pdev)
{
    int i = 0;
    enum gpiod_flags flags = GPIOD_IN;
    gpio_num = gpiod_count(&pdev->dev, "user-keys");
    printk("gpio_num is %d\n",gpio_num);
    device_infos = kmalloc(sizeof(struct device_info) * gpio_num, GFP_KERNEL);
    for (i = 0; i < gpio_num; i++)
    {
        device_infos[i].gpio_num = i;
        gpio_info = gpiod_get_index(&pdev->dev, "user-keys", i,flags);
        device_infos[i].gpio_info = gpio_info;
        if (IS_ERR(gpio_info))
        {
            printk("get gpio_info failed\n");
            return -1;
        }
        create_device_point(i);
        printk("get gpio_info success\n");
    }

    return 0;
}

int led_remove(struct platform_device * pdev)
{
    int i = 0;
    for (i =0; i < gpio_num; i++)
    {
        destroy_device(i);
        gpiod_put(device_infos[i].gpio_info);
    }
    kfree(device_infos);
    return 0;
}

static const struct of_device_id led_table[] = {
	{ .compatible = "user-keys"},
};

static struct platform_driver led_driver = {
    .probe      = led_probe,
    .remove     = led_remove,
    .driver     = {
    .name   = "jsdfhasuh_led",
    .of_match_table = led_table,
    },
};

static int __init led_drv_init(void)
{
    int err;
    
    err = platform_driver_register(&led_driver); 
    printk("success load boaed_a_led_drv\n");
    give_p_led_opr(&board_demo_led_opr);
    return 0;
}

static void __exit led_drv_exit(void)
{
    platform_driver_unregister(&led_driver);
}

module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");

