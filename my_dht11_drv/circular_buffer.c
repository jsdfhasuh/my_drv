#include "circular_buffer.h"
#include "linux/printk.h"
#include <linux/slab.h>

//先进先出的环形缓冲区
void init_c_buffer(c_buffer_ptr * c_buffer_ptr_ptr,int buffer_size)
{
    c_buffer_ptr temp_ptr;
    *c_buffer_ptr_ptr= kmalloc(sizeof(c_buffer),GFP_KERNEL);
    if (!*c_buffer_ptr_ptr){
        printk("c_buffer malloc fail");
        return;
    }
    temp_ptr = *c_buffer_ptr_ptr;
    temp_ptr ->buffer_size = buffer_size;
    temp_ptr ->read =0;
    temp_ptr ->write =0;
    temp_ptr ->buffer_data = (char*) kmalloc(buffer_size*sizeof(char), GFP_KERNEL);
    if (!(*c_buffer_ptr_ptr)->buffer_data) 
    {
        // 处理分配失败
        printk("c_buffer_data malloc fail");
        kfree(*c_buffer_ptr_ptr);
        *c_buffer_ptr_ptr = NULL;
        return;
    }
    printk("c_buffer malloc success and init succcess");
}

void free_c_buffer(c_buffer_ptr * c_buffer_ptr_ptr)
{
    kfree((*c_buffer_ptr_ptr) ->buffer_data);
    kfree((*c_buffer_ptr_ptr));
    *c_buffer_ptr_ptr = NULL;
}


static int next_pos(c_buffer_ptr c_buffer_ptr,int x)
{
    return ((x+1) % c_buffer_ptr->buffer_size);
}

// 只有在读指针与写指针重合的时候
static int is_cbuf_empty(c_buffer_ptr c_buffer_ptr)
{
    return ((c_buffer_ptr ->read) == (c_buffer_ptr -> write));
}

static int is_cbuf_full(c_buffer_ptr c_buffer_ptr)
{
    return ((c_buffer_ptr ->read) == (next_pos(c_buffer_ptr, c_buffer_ptr ->write)));
}

int put_data(c_buffer_ptr c_buffer_ptr,char data)
{
    int status = 1;
    if (!is_cbuf_full(c_buffer_ptr))
    {
        status = 0;
        (c_buffer_ptr ->buffer_data)[c_buffer_ptr->write] = data;
        c_buffer_ptr ->write = next_pos(c_buffer_ptr, c_buffer_ptr->write);
    }
    return status;
}

int get_data(c_buffer_ptr c_buffer_ptr,char *data_ptr)
{
    int status = 1;
    if (!is_cbuf_empty(c_buffer_ptr))
    {
        status = 0;
        *data_ptr = (c_buffer_ptr->buffer_data)[c_buffer_ptr->read];
        c_buffer_ptr->read = next_pos(c_buffer_ptr, c_buffer_ptr -> read);
    }
    return status;
}


int get_free_volumn(c_buffer_ptr c_buffer_ptr)
{
    if (c_buffer_ptr->write == c_buffer_ptr->read)
    {
        return c_buffer_ptr->buffer_size;
    }
    if (c_buffer_ptr->write > c_buffer_ptr->read) 
    {
        return (c_buffer_ptr->buffer_size) - ((c_buffer_ptr->write) - (c_buffer_ptr->read) + 1) ;
    } 
    else 
    {
        return (c_buffer_ptr->read) - (c_buffer_ptr->write) - 1;
    }
}

