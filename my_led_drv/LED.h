#ifndef _LED_H
#define _LED_H

struct led_op{
    int (*init)(int which,char io_status);
    int (*ctl)(int which,char status);
    int (*read)(int which);
};

struct led_op* get_board_led_op(void);


#endif