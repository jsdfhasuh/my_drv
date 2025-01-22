#ifndef _LED_H
#define _LED_H
struct gpio_op{
    int (*init)(int which,char io_status);
    int (*ctl)(int which,char status);
    int (*read)(int which);
    int (*wait)(int which);
};


void create_device_point(int minor);
void destroy_device(int minor);
void give_p_led_opr(struct gpio_op *opr);






#endif