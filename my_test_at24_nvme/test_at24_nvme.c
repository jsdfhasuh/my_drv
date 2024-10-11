#include "linux/types.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#define EEPROM_PATH "/sys/bus/nvmem/devices/0-00500/nvmem"
#define BUFFER_SIZE 256


void print_buffer(const char *buffer,size_t length)
{
    for (size_t i = 0;i < length;i++)
    {
        printf("%02x",buffer[i]);
    }
    printf("\n");
}

int read_eeprom(size_t offset,size_t length,void *buffer,int fd)
{
    if (pread(fd, buffer,length,offset) != length)
    {
        printf("error read EEPROM");
        return -1;
    }
    
}

int write_eeprom(size_t offset,size_t length,void *buffer,int fd)
{
    if (pwrite(fd, buffer, length, offset) != length)
    {
        printf("error write EEPROM");
        return -1;
    }
}

int main(void)
{
    char read_buffer[BUFFER_SIZE] = {'0'};
    char write_buffer[] ={0x01,0x02};
    size_t offset = 0;
    size_t length = BUFFER_SIZE;
    int error;
    int fd = open(EEPROM_PATH, O_RDWR);
    if (fd < 0)
    {
        printf("error open EEPROM");
        return -1;
    }
    error = read_eeprom(offset, 10, read_buffer, fd);
    if (error < 0)
    {
        return -1;
    }
    print_buffer(read_buffer,10);
    write_eeprom(offset, 2, write_buffer, fd);
    read_eeprom(offset, 10, read_buffer, fd);
    print_buffer(read_buffer,10);
    return 0;
}