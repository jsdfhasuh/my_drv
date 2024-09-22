#include "asm-generic/errno-base.h"
#include "asm-generic/gpio.h"
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
#include "jsdfhasuh_cdev.h"
#include "circular_buffer.h"
#include "net/datalink.h"
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/gpio/driver.h> // 包含 gpiod_to_chip 和 gpiod_line 函数的声明

gpio_resource * dht11_gpios;
//创建一系列的环形缓冲区
c_buffer_ptr * c_buffer_ptrs;
int dht11_num;
gpio_op * dht11_op;
int return_status = 0 ;
int g_dht11_irq_cnt = 0;
int g_dht11_irq_time[100]={0};
unsigned char return_datas[5];

static int parse_dht11_datas_save(int which)
{
	int i;
	u64 high_time;
	unsigned char data = 0;
	int bits = 0;
	int byte = 0;
    int free_volumn;
	unsigned char crc;
    unsigned char datas[5]={0};
 
	/* 数据个数: 可能是81、82、83、84 */
	if (g_dht11_irq_cnt < 80)
	{
		/* 出错 */
		// 唤醒APP
		g_dht11_irq_cnt = 0;
        return_status =1;
		return 0;
	}
 
	// 解析数据
	for (i = g_dht11_irq_cnt - 80; i < g_dht11_irq_cnt-1; i+=2)  // 数据的末端减去存在的81个，但是因为g_dht11_irq_cnt是存在数据个数的减1
	{
		high_time = g_dht11_irq_time[i] - g_dht11_irq_time[i-1];
 
		data <<= 1;
 
		if (high_time > 50000) /* data 1 */
		{
			data |= 1;
		}
 
		bits++;
 
		if (bits == 8)
		{
			datas[byte] = data;
			data = 0;
			bits = 0;
			byte++;
		}
	}
 
	crc = datas[0] + datas[1] + datas[2] + datas[3];
    g_dht11_irq_cnt = 0;
	
    return_status = 1;
	if (crc == datas[4])
	{
        printk("right\n");
        free_volumn = get_free_volumn(c_buffer_ptrs[which]);
        if (free_volumn >= 5)
        {
            for (i =0;i<5;i++)
                put_data(c_buffer_ptrs[which],datas[i]);
        }
        return 1;
    }
    return 0;
}

static irqreturn_t dht11_handler(int IRQ, void * pdevice)
{
	//struct gpio_desc *gpio_desc = dev_id;
	u64 time;
	/* 1. 记录中断发生的时间 */
	time = ktime_get_raw_ns();
	g_dht11_irq_time[g_dht11_irq_cnt] = time;
	/* 2. 累计次数 */
	g_dht11_irq_cnt++;
    if (g_dht11_irq_cnt == 83)
	{
        //printk("g_dht11_irq_cnt is %d",g_dht11_irq_cnt);
        gpio_resource * dht11_gpio = (gpio_resource *) pdevice;
		return_status = 1;
        wake_up_interruptible(dht11_gpio ->wait_queue_head_ptr);
	}
	return IRQ_HANDLED;
}

static void key_timer_expire(unsigned long data)
{

    gpio_resource * dht11_gpio;
    dht11_gpio = (gpio_resource *)data;
    printk("timeout\n");
    return_status = 1;
    wake_up_interruptible(dht11_gpio ->wait_queue_head_ptr);
}

static void async_timer(unsigned long data)
{
    int which;
    which = (int)data;
    
}

static int dht11_init(int which,char io_status)
{   
    printk("init DHT11");
    return 0;
}

static int dht11_ctl(int which,char io_status)
{
    printk("ctl DHT11");
    return 0;
}

static int dht11_write(int which,int io_status)
{
    int error;
    printk("write DHT11 %d %d",which,io_status);
    error = gpiod_direction_output(dht11_gpios[which].gpio_info, io_status);
    return 0;
}

static int dht11_read(int which,char * * data_ptr_ptr,unsigned long * data_len_ptr)
{
    // 获取开始时间
    int i=0;
    int dht11_status = 0;
    int err;
    return_status = 0;
    memset(g_dht11_irq_time, 0, sizeof(g_dht11_irq_time));
    memset(return_datas, 0, sizeof(return_datas));
    printk("read DHT11 %d\n",which);
    err = gpiod_direction_output(dht11_gpios[which].gpio_info,0); //拉低电平，发送开始信号
    gpiod_set_value(dht11_gpios[which].gpio_info, 0);
    mdelay(20);
    err = gpiod_direction_input(dht11_gpios[which].gpio_info);
    err = request_irq(dht11_gpios[which].irq,dht11_handler,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,"jsdfhasuh_dht11", &(dht11_gpios[which])); //注册中断'
    if (err <0)
    {
        printk("request_irq failed\n");
        return -1;
    }
    mod_timer(&(dht11_gpios[which].key_timer), jiffies + 300);	
 
	/* 3. 休眠等待数据 */
	wait_event_interruptible(*(dht11_gpios[which].wait_queue_head_ptr), return_status);
    del_timer(&(dht11_gpios[which].key_timer));
    dht11_status = parse_dht11_datas_save(which);
    free_irq(dht11_gpios[which].irq, &dht11_gpios[which]);
    for (i = 0; i<100;i++)
        // int g_dht11_irq_time[100]={0};
        printk("irq_time is %d",g_dht11_irq_time[i]);
    if (dht11_status)
        for (i = 0; i<5; i++)
            get_data(c_buffer_ptrs[which], &return_datas[i]);
    //parse_dht11_datas_save();
    * data_ptr_ptr = return_datas;
    * data_len_ptr = sizeof(return_datas);
    return dht11_status;
}

int dht11_probe (struct platform_device * pdev)
{
    int i = 0;
    int IRQ = 0;
    int error = 0;
    enum gpiod_flags flags = GPIOD_IN;
    dht11_num = gpiod_count(&pdev->dev, "dht11");
    printk("dht11_num is %d\n",dht11_num);
    dht11_gpios = (gpio_resource *)kmalloc(dht11_num*sizeof(gpio_resource),GFP_ATOMIC);

    c_buffer_ptrs = (c_buffer_ptr *)kmalloc(dht11_num*sizeof(c_buffer_ptr), GFP_ATOMIC);
    for (i = 0; i < dht11_num; i++)
    {
        printk("dht11_num is %d\n",i);
        // 动态分配等待队列
        dht11_gpios[i].wait_queue_head_ptr = (wait_queue_head_t *)kmalloc(sizeof(wait_queue_head_t), GFP_ATOMIC);
        if (!dht11_gpios[i].wait_queue_head_ptr) 
        {
            printk("Failed to allocate memory for wait queue\n");
            return -ENOMEM;
        }
        init_waitqueue_head(dht11_gpios[i].wait_queue_head_ptr);

        dht11_gpios[i].gpio_info = gpiod_get_index(&pdev->dev, "dht11", i,flags);
        if (IS_ERR(dht11_gpios[i].gpio_info)) 
        {
            dev_err(&pdev->dev, "Failed to get GPIO descriptor\n");
            return PTR_ERR(dht11_gpios[i].gpio_info);
        }
        // register inttrupt
        //IRQ = gpiod_to_irq(dht11_gpios ->gpio_info);
        IRQ = gpiod_to_irq(dht11_gpios ->gpio_info);
        if (IRQ < 0)
        {
            printk("get IRQ failed\n");
            return -1;
        }
        dht11_gpios[i].irq =IRQ;
        setup_timer(&(dht11_gpios[i].key_timer), key_timer_expire, (unsigned long)(&(dht11_gpios[i])));
        //error = request_irq(dht11_gpios[i].irq,dht11_handler,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,"jsdfhasuh_dht11", &dht11_gpios[i]);
        if (error < 0)
        {
            printk("request_irq failed\n");
            return -1;
        }
        printk("get gpio_info success and register IRQ success\n");
        init_c_buffer(&c_buffer_ptrs[i], 10);
    }
    return 0;
}

int dht11_remove(struct platform_device * pdev)
{
    int i;
    for (i = 0; i < dht11_num; i++) 
    {
        //free_irq(dht11_gpios[i].irq, &dht11_gpios[i]);
        gpiod_put(dht11_gpios[i].gpio_info);
        kfree(dht11_gpios[i].wait_queue_head_ptr);
        destroy_device(i);
        jsdfhasuh_unregister_chrdev();
        give_gpio_opr(NULL, NULL);
    }
    kfree(dht11_gpios);
    printk("DHT11 device removed\n");
    return 0;
}

static const struct of_device_id dht11_table[] = {
	{ .compatible = "dht11"},
};

static struct platform_driver dht11_driver = {
    .probe      = dht11_probe,
    .remove     = dht11_remove,
    .driver     = {
    .name   = "jsdfhasuh_dht11",
    .of_match_table = dht11_table,
    },
};



static int __init dht11_drv_init(void)
{
    int err;
    int i;
    err = platform_driver_register(&dht11_driver); 
    dht11_op = (gpio_op *)kmalloc(sizeof(gpio_op), GFP_ATOMIC);
    dht11_op ->init= dht11_init;
    dht11_op ->read = dht11_read;
    dht11_op ->ctl = dht11_ctl;
    dht11_op ->write = dht11_write;
    dht11_op ->dev_name = "DHT11";
    give_gpio_opr(dht11_op,dht11_gpios);
    jsdfhasuh_register_chrdev();
    for (i = 0;i<dht11_num;i++)
        create_device_point(i);
    printk("success load boaed_a_dht11_drv\n");
    //give_p_led_opr(&board_demo_led_opr);
    return 0;
}


static void __exit dht11_drv_exit(void)
{
    platform_driver_unregister(&dht11_driver);
}

module_init(dht11_drv_init);
module_exit(dht11_drv_exit);
MODULE_AUTHOR("jsdfhasuh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DHT11");
