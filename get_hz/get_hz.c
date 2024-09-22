#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/param.h>

static int __init my_module_init(void) {
    printk(KERN_INFO "HZ: %d\n", HZ);
    printk(KERN_INFO "Each jiffy represents: %lf seconds\n", 1.0 / HZ);
    return 0;
}

static void __exit my_module_exit(void) {
    printk(KERN_INFO "Module exiting\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module to print HZ and jiffies duration");
MODULE_AUTHOR("Your Name");