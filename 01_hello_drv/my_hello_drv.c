#include "linux/printk.h"
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


//确定主设备号，也可以让内核分配
static int major_device_num = 0;
static struct class *hello_class;

int drv_open (struct inode *, struct file *);
ssize_t drv_read (struct file *, char __user *, size_t, loff_t *);
ssize_t drv_write (struct file *, const char __user *, size_t, loff_t *);
int drv_release (struct inode *, struct file *);

//定义file_operations 结构体
static struct file_operations hello_drv = {
	.owner	 = THIS_MODULE,
	.open    = drv_open,
	.read    = drv_read,
	.write   = drv_write,
	.release = drv_release,
};

//实现对应的 drv_open/drv_read/drv_write 等函数，填入file_operations 结构体

int drv_open (struct inode *inode,struct file *file)
{
    printk("hello this jsdfhasuh");
}

ssize_t drv_read (struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    printk("hello this jsdfhasuh");
}

ssize_t drv_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    printk("hello this jsdfhasuh");
}

int drv_release (struct inode *inode, struct file *file)
{
    printk("hello this jsdfhasuh");
}

//把file_operations 结构体告诉内核：register_chrdev,注册驱动程序，得有一个入口函数：安装驱动程序时，就会去调用这个入口函数
static int __init hello_init(void)
{
	int err;
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	major_device_num = register_chrdev(0, "hello", &hello_drv);  /* /dev/hello */


	hello_class = class_create(THIS_MODULE, "hello_class");
	err = PTR_ERR(hello_class);
	if (IS_ERR(hello_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major_device_num, "hello");
		return -1;
	}
	
	device_create(hello_class, NULL, MKDEV(major_device_num, 0), NULL, "hello"); /* /dev/hello */
	
	return 0;
}

//卸载驱动程序时，出口函数调用unregister_chrdev
static void __exit hello_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(hello_class, MKDEV(major_device_num, 0));
	class_destroy(hello_class);
	unregister_chrdev(major_device_num, "hello");
}
//其他完善：提供设备信息，自动创建设备节点：class_create,device_create
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
