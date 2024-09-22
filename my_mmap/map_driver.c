#include "asm/uaccess.h"
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
#include <asm/pgtable.h>
#include <linux/mm.h>
#include <linux/slab.h>



#define MIN(a, b) (a < b ? a : b)
static int major =0;
struct map_driver_info {
    int bufsiz;
    struct mutex map_mutex;
    char * buf_ptr;
    struct class *cls;
};

struct map_driver_info* my_map_driver_info;
static int map_driver_open(struct inode *inode, struct file *fd)
{
    mutex_lock(&(my_map_driver_info->map_mutex));
    if (!(my_map_driver_info->buf_ptr))
    {
        printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        my_map_driver_info->buf_ptr = kzalloc(1024*8, GFP_KERNEL);
        if (!(my_map_driver_info->buf_ptr)) {
            printk("%s %s line %d: Memory allocation failed\n", __FILE__, __FUNCTION__, __LINE__);
            return -ENOMEM;
        }
    }
    else {
        printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        printk("buf_ptr is not NULL\n");
    }
    mutex_unlock(&(my_map_driver_info->map_mutex));
    return 0;
}

static int map_driver_release(struct inode *inode, struct file *fd)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t map_driver_read (struct file *fd, char __user * user_buff, size_t size, loff_t * loft)
{
    int err;
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    mutex_lock(&(my_map_driver_info->map_mutex));
    err = copy_to_user(user_buff,my_map_driver_info->buf_ptr, MIN(size, 1024*8));
    if (err) 
    {
        printk("%s %s line %d: copy_to_user failed\n", __FILE__, __FUNCTION__, __LINE__);
        return -EFAULT;
    }
    mutex_unlock(&(my_map_driver_info->map_mutex));
    return 0;
}

static ssize_t map_driver_write (struct file *fd, const char __user * user_buff, size_t size, loff_t * loft)
{
    int err;
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    mutex_lock(&(my_map_driver_info->map_mutex));
    err = copy_from_user(my_map_driver_info->buf_ptr, user_buff, MIN(size, 1024*8));
    if (err) 
    {
        printk("%s %s line %d: copy_from_user failed\n", __FILE__, __FUNCTION__, __LINE__);
        return -EFAULT;
    }
    return 0;
}

int map_driver_mmap (struct file * fd, struct vm_area_struct * vm)
{
    unsigned long phy = virt_to_phys(my_map_driver_info->buf_ptr);
    mutex_lock(&(my_map_driver_info->map_mutex));
    vm -> vm_page_prot = pgprot_writecombine(vm->vm_page_prot);
    if (remap_pfn_range(vm, vm->vm_start, phy >> PAGE_SHIFT, vm->vm_end - vm->vm_start, vm->vm_page_prot)) {
        printk("mmap remap_pfn_range failed\n");
        return -ENOBUFS;
    }
    mutex_unlock(&(my_map_driver_info->map_mutex));
    return 0;
}

static struct file_operations map_drv = {
	.owner	 = THIS_MODULE,
	.open    = map_driver_open,
	.read    = map_driver_read,
	.write   = map_driver_write,
	.release = map_driver_release,
	.mmap    = map_driver_mmap,
};

static int __init map_drv_init(void)
{
    int errno;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    my_map_driver_info = kzalloc(sizeof(struct map_driver_info), GFP_KERNEL); // 使用 kmalloc 分配内存
	major = register_chrdev(0, "hello", &map_drv);  /* /dev/hello */
    mutex_init(&(my_map_driver_info->map_mutex));
	my_map_driver_info->cls = class_create(THIS_MODULE, "map_class");
	errno = PTR_ERR(my_map_driver_info->cls);
	if (IS_ERR(my_map_driver_info->cls)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "map_class");
		return -1;
	}
	
	device_create(my_map_driver_info->cls, NULL, MKDEV(major, 0), NULL, "jsdfhasuh_map"); /* /dev/hello */
	
	return 0;
}


static void __exit map_drv_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(my_map_driver_info->cls, MKDEV(major, 0));
	class_destroy(my_map_driver_info->cls);
	unregister_chrdev(major, "map_class");
	kfree(my_map_driver_info->buf_ptr);
    kfree(my_map_driver_info);
}

module_init(map_drv_init);
module_exit(map_drv_exit);