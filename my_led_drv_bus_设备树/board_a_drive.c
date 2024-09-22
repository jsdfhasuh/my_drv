
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
#include "linux/of.h"
#include "linux/printk.h"
#include "LED.h"
#include <asm/io.h>
#include <linux/platform_device.h>


int io_status = 0; // 0 为输入 1 为输出

struct Pin_address {
    unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;
    unsigned int *GPIO5_GDIR;
    unsigned int *GPIO5_DR;
};

static struct Pin_address GPIO_address[100];

int board_led_init(int which,char status)
{

    if (!(GPIO_address[which].IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3))
    {
        printk("IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 is null\n");
        GPIO_address[which].IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x2290000+0x14,4);
        printk("enable gpio5\n");
        *(GPIO_address[which].IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3) &= ~0xf;
        *(GPIO_address[which].IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3) |= 0x5;
    }
    if (!GPIO_address[which].GPIO5_GDIR)
    {
        printk(" GPIO5_GDIR is null\n");
        GPIO_address[which].GPIO5_GDIR = ioremap(0x20AC004,4);
    }
    if (!GPIO_address[which].GPIO5_DR)
    {
        GPIO_address[which].GPIO5_DR = ioremap(0x20AC000,4);
    }
    // enable gpio5
    printk("IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 is %x\n",*(GPIO_address[which].IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3));
    if (status == 1)
    {   
        *(GPIO_address[which].GPIO5_GDIR) |= (1<<3);
        printk(" GPIO5_GDIR is %x\n",*(GPIO_address[which].GPIO5_GDIR));
    }
    else
    {
        *(GPIO_address[which].GPIO5_GDIR) &= ~(1<<3);
        printk(" GPIO5_GDIR is %x\n",*(GPIO_address[which].GPIO5_GDIR));

    }

    printk("which is %d,status is %d",which,status);
    return 0;
}

int board_led_control(int which,char status)
{
    printk("which is %d,status is %d,\n",which,status);
    board_led_init(which,1);
    if (status == 1)
    {
        *(GPIO_address[which].GPIO5_DR) |= (1<<3);
    }
    else
    {
        *(GPIO_address[which].GPIO5_DR) &= ~(1<<3);
    }
    printk(" GPIO5_DR is %x\n",*(GPIO_address[which].GPIO5_DR));
    return 0;
}

int board_led_read(int which)
{
    printk("which is %d",which);
    board_led_init(which, 0);
    printk("GPIO5_DR is %x\n",*(GPIO_address[which].GPIO5_DR));
    if ((*(GPIO_address[which].GPIO5_DR) & (1<<3)) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static struct led_op board_demo_led_opr = {
	.init = board_led_init,
	.ctl  = board_led_control,
    .read = board_led_read,
};

struct led_op* get_board_led_op(void)
{
    give_p_led_opr(&board_demo_led_opr);
}

int led_probe (struct platform_device * pdev)
{
    unsigned int GPIO5_GDIR_add, GPIO5_DR_add, IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3_add;
    struct device_node *np;
    int led_num = 0;
    int err;
    printk("began to get resource\n");
    np = pdev->dev.of_node;
    if (!np)
    {
        return -1;
    }
    err = of_property_read_u32(np, "num", &led_num);
    printk("led_num is %d\n",led_num);

    err = of_property_read_u32(np, "GDIR", &GPIO5_GDIR_add);
    err = of_property_read_u32(np, "DR", &GPIO5_DR_add);
    err = of_property_read_u32(np, "IOMUX", &IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3_add);
    printk("GPIO5_GDIR_add is %x\n GPIO5_DR_add is %x\n IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3_add is %x\n", GPIO5_GDIR_add,GPIO5_DR_add,IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3_add);
    
    GPIO_address[led_num].IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3_add,4);
    GPIO_address[led_num].GPIO5_GDIR = ioremap(GPIO5_GDIR_add,4);
    GPIO_address[led_num].GPIO5_DR = ioremap(GPIO5_DR_add,4);
    create_device_point(led_num);

    return 0;
}

int led_remove(struct platform_device * pdev)
{
    struct device_node *np;
    int led_num = 0;
    int err;
    printk("began to remove resource\n");
    np = pdev->dev.of_node;
    err = of_property_read_u32(np, "num", &led_num);
    printk("led_num is %d\n",led_num);
    destroy_device(led_num);
    return 0;
}

static const struct of_device_id led_table[] = {
	{ .compatible = "gpio-leds"},
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

