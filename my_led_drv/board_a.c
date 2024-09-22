
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
#include "LED.h"
#include <asm/io.h>

int io_status = 0; // 0 为输入 1 为输出

/* registers */
// IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 地址：0x02290000 + 0x14
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;

// GPIO5_GDIR 地址：0x020AC004
static volatile unsigned int *GPIO5_GDIR;

//GPIO5_DR 地址：0x020AC000
static volatile unsigned int *GPIO5_DR;


int board_led_init(int which,char status)
{

    switch (which)
    {
        case 0:
        {
            if (!IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3)
            {
                printk("IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 is null\n");
                IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x2290000+0x14,4);
                printk("enable gpio5\n");
                *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 &= ~0xf;
                *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 |= 0x5;
            }
            if (!GPIO5_GDIR)
            {
                printk(" GPIO5_GDIR is null\n");
                GPIO5_GDIR = ioremap(0x20AC004,4);
            }
            if (!GPIO5_DR)
            {
                GPIO5_DR = ioremap(0x20AC000,4);
            }
            // enable gpio5
            printk("IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 is %x\n",*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3);
            if (status == 1)
            {   
                *GPIO5_GDIR |= (1<<3);
                printk(" GPIO5_GDIR is %x\n",*GPIO5_GDIR);
            }
            else
            {
                *GPIO5_GDIR &= ~(1<<3);
                printk(" GPIO5_GDIR is %x\n",*GPIO5_GDIR);

            }
        }
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
        *GPIO5_DR |= (1<<3);
    }
    else
    {
        *GPIO5_DR &= ~(1<<3);
    }
    printk(" GPIO5_DR is %x\n",*GPIO5_DR);
    return 0;
}

int board_led_read(int which)
{
    printk("which is %d",which);
    board_led_init(which, 0);
    printk("GPIO5_DR is %x\n",*GPIO5_DR);
    if ((*GPIO5_DR & (1<<3)) != 0)
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
    return &board_demo_led_opr;    
}

