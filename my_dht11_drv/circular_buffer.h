#ifndef CIRCULAR_Buffer_H
#define CIRCULAR_Buffer_H

typedef struct c_buffer
{
    int read;
    int write;
    int buffer_size;
    char *buffer_data;
} c_buffer,* c_buffer_ptr;

void init_c_buffer(c_buffer_ptr * c_buffer_ptr_ptr,int buffer_size);
void free_c_buffer(c_buffer_ptr * c_buffer_ptr_ptr);
int put_data(c_buffer_ptr c_buffer_ptr,char data); 
int get_data(c_buffer_ptr c_buffer_ptr,char *data_ptr);
int get_free_volumn(c_buffer_ptr c_buffer_ptr);
#endif