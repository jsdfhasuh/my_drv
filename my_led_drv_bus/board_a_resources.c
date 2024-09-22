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
#include <linux/platform_device.h>

#include "gpio.h"
#include "linux/ioport.h"

static void led_dev_release(struct device *dev)
{

}

static struct resource resource_test[] = {
        {
                .start = 0x2290000+0x14,
                .end = 0x2290000+0x14+4,
                .flags = IORESOURCE_MEM,
                .name = "IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3",
                .sibling = &resource_test[1],
        },
        {
                .start = 0x20AC004,
                .end = 0x20AC004+4,
                .flags = IORESOURCE_MEM,
                .name = "GPIO5_GDIR",
                .sibling = &resource_test[2],
        },
        {
                .start = 0x20AC000,
                .end = 0x20AC000+4,
                .flags = IORESOURCE_MEM,
                .name = "GPIO5_DR",
        },
};

static struct resource main_resource[] = {
        [0] = {
                .flags = IORESOURCE_IO,
                .name  = "led_dev",
                .child = &resource_test[0],
        }       
};


static struct platform_device board_A_led_dev = {
        .name = "jsdfhasuh_led",
        .num_resources = ARRAY_SIZE(main_resource),
        .resource = main_resource,
        .dev = {
                .release = led_dev_release,
         },
};


static int __init led_dev_init(void)
{
    int err;
    
    err = platform_device_register(&board_A_led_dev);   
    printk("success load board_a_resources\n");
    return 0;
}

static void __exit led_dev_exit(void)
{
    platform_device_unregister(&board_A_led_dev);
}

module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_LICENSE("GPL");