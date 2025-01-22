#ifndef _JSDFHASUH_H
#define _JSDFHASUH_H
#include <linux/timer.h>
#include "linux/fs.h"
#include "linux/wait.h"

typedef struct gpio_op{
    char * dev_name;
    int (*init)(int which,char io_status);
    int (*ctl)(int which,char status);
    int (*read)(int which,char * *data_ptr_ptr,unsigned long * n_ptr);
    int (*write)(int which,int value);
    int (*fasync)(int which,int on);
} gpio_op;


typedef struct gpio_resource{
    int gpio;
    int irq;
    char *name;
    int num;
    int key;
    struct gpio_desc *gpio_info;
    struct timer_list key_timer;
    struct timer_list async_timer;
    wait_queue_head_t * wait_queue_head_ptr;
    struct fasync_struct *gpio_fasync;
    struct work_struct work;
    spinlock_t lock;
} gpio_resource;


void create_device_point(int minor);
void destroy_device(int minor);
void give_gpio_opr(struct gpio_op *opr,gpio_resource * gpio_resource_ptr);
int jsdfhasuh_register_chrdev(void);
int jsdfhasuh_unregister_chrdev(void);



#endif