
#include "asm/uaccess.h"
#include "linux/export.h"
#include "linux/printk.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/property.h>
#include <linux/acpi.h>
#include <linux/i2c.h>
#include <linux/nvmem-provider.h>
#include <linux/regmap.h>
#include <linux/pm_runtime.h>
#include <linux/gpio/consumer.h>
#include <linux/uaccess.h>
#include <linux/fs.h>


static int major = 0;
static struct class * lm75_class;
static struct i2c_client *lm75a_client;

static ssize_t lm75_read(struct file * file,char __user *buf,size_t size,loff_t *offset)
{
    int error;
    char * p;
    char temp;
    int val;
    int result;
    int complement;
    val = i2c_smbus_read_word_data(lm75a_client,  0x00);
    printk("val is %02x",val);
    //parse data
    p = (char*)&val;
    temp = p[0];
    p[0] = p[1];
    p[1] = temp;
    val = val >> 5;
    printk("final val is %02x",val);
    if (*(p+1) & 0x04)
    {
        printk("negative");
        complement = ~val + 1;
        result = complement;
    }
    else
    {
        result = val ;
    }
    error = copy_to_user(buf, &result, sizeof(result));
    return 0;
}

static int lm75_open(struct inode *node,struct file *file)
{
    return 0;
}

static int lm75_release(struct inode * node, struct file * file)
{
    return 0;
}

static struct file_operations lm75_ops={
    .owner = THIS_MODULE,
    .open = lm75_open,
    .read = lm75_read,
    .release = lm75_release,
};


// register i2c_driver
static const struct i2c_device_id lm75_ids[]={
    [0] = {.name = "lm75a",.driver_data=(kernel_ulong_t)NULL},
    [1] = {}
};

static const struct of_device_id of_match_ids_lm75[] = {
	{ .compatible = "lm75a",		.data = NULL },
	{ /* END OF LIST */ },
};


int lm75_probe(struct i2c_client * client, const struct i2c_device_id * id)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	lm75a_client = client;
	/* register_chrdev */
	major = register_chrdev(0, "lm75", &lm75_ops);
	lm75_class = class_create(THIS_MODULE, "lm75");
	device_create(lm75_class, NULL, MKDEV(major, 0), NULL, "lm75a"); /* /dev/ap3216c */
    return 0;
}
int lm75_remove(struct i2c_client * client)
{

    return 0;
}

static struct i2c_driver i2c_lm75_driver = {
    .driver ={
        .name = "lm75a",
        .of_match_table = of_match_ids_lm75
    },
    .probe = lm75_probe,
	.remove = lm75_remove,
	.id_table = lm75_ids,
};

static  int __init i2c_driver_lm75_init(void)
{
    printk("%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    return i2c_add_driver(&i2c_lm75_driver);
}
module_init(i2c_driver_lm75_init);

static void __exit i2c_driver_lm75_exit(void)
{
    device_destroy(lm75_class,MKDEV(major, 0));
    class_destroy(lm75_class);
    unregister_chrdev(major, "lm75a");
    i2c_del_driver(&i2c_lm75_driver);
    printk("%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    return;
}

module_exit(i2c_driver_lm75_exit);

MODULE_AUTHOR("jsdfhasuh");
MODULE_LICENSE("GPL");
