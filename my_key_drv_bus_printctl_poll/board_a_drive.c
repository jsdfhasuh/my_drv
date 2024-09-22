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
#include <linux/interrupt.h>
#include "linux/printk.h"
#include "gpio.h"

#include <asm/io.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>

wait_queue_head_t *  gpio_wait_q ; 
int io_status = 0; // 0 为输入 1 为输出
int gpio_num = 0;
struct device_info{
    int gpio_num;
    int gpio_status;
    int gpio_value;    
    struct gpio_desc *gpio_info;
    int IRQ;
    wait_queue_head_t *wait_queue_pr;
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

int wait_data(int which)
{
    wait_event_interruptible(*device_infos[which].wait_queue_pr, gpiod_get_value(device_infos[which].gpio_info));
    return 0;
}

static struct gpio_op board_demo_led_opr = {
	.init = board_led_init,
	.ctl  = board_led_control,
    .read = board_led_read,
    .wait = wait_data,
};

int get_board_led_op(void)
{
    give_p_led_opr(&board_demo_led_opr);
    return 0;
}

static irqreturn_t gpio_key_handler(int IRQ, void * pdevice)
{
    int value,which;
    struct device_info *device_info = (struct device_info *) pdevice;
    which = device_info->gpio_num;
    printk("gpio_key is %d\n",device_info -> gpio_num);
    value = gpiod_get_value(device_info->gpio_info);
    printk("value is %d\n",value);
    if (waitqueue_active(device_infos[which].wait_queue_pr))
    {
        printk("wake up\n");
        wake_up_interruptible(device_infos[which].wait_queue_pr);
    }
    else {
        printk("no wake up\n");
    }
    return IRQ_HANDLED;
}



int led_probe (struct platform_device * pdev)
{
    int i = 0;
    int IRQ = 0;
    int error = 0;
    enum gpiod_flags flags = GPIOD_IN;
    gpio_wait_q = get_wait_queue();
    printk("gpio_wait_q is %p\n",gpio_wait_q);
    gpio_num = gpiod_count(&pdev->dev, "user-keys");
    printk("gpio_num is %d\n",gpio_num);
    device_infos = kmalloc(sizeof(struct device_info) * gpio_num, GFP_KERNEL);
    for (i = 0; i < gpio_num; i++)
    {
        printk("gpio_num is %d\n",i);
        device_infos[i].gpio_num = i;
        gpio_info = gpiod_get_index(&pdev->dev, "user-keys", i,flags);
        // register inttrupt
        IRQ = gpiod_to_irq(gpio_info);
        printk("IRQ is %d\n",IRQ);
        if (IRQ < 0)
        {
            printk("get IRQ failed\n");
            return -1;
        }
        device_infos[i].IRQ = IRQ;
        device_infos[i].gpio_info = gpio_info;
        device_infos[i].wait_queue_pr = gpio_wait_q;
        
        error = request_irq(device_infos[i].IRQ,gpio_key_handler,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,"100ask_gpio_key", &device_infos[i]);
        if (error < 0)
        {
            printk("request_irq failed\n");
            return -1;
        }
        if (IS_ERR(gpio_info))
        {
            printk("get gpio_info failed\n");
            return -1;
        }
        create_device_point(i);
        printk("get gpio_info success and register IRQ success\n");
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
        free_irq(device_infos[i].IRQ, &device_infos[i]);
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

