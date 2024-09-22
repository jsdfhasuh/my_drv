#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd;
    int status;
    int i;
    unsigned char datas[5];
    if (argc != 3)
    {
        printf("Usage: %s <dev> <on | off>\n", argv[0]);
		return -1;
    }
    fd = open(argv[1],O_RDWR | O_NONBLOCK);
    if (fd == -1)
    {
        printf("can not open file %s\n", argv[1]);
		return -1;
    }
    if (0 == strcmp(argv[2], "on"))
	{
		status = 0;
		write(fd, &status, 1);
	}
	else if (0 == strcmp(argv[2], "off"))
	{
		status = 1;
		write(fd, &status, 1);
	}
    else if ( 0 == strcmp(argv[2],"read"))
    {
        while (1) 
        {
            // 读取设备状态
            printf("begin\n");
            read(fd, datas, sizeof(datas));
            for (i=0;i<5;i++)
                printf("value is %d\n",datas[i]);
            // 等待一段时间（例如 1 秒）
            //sleep(1); // 1000000 微秒 = 1 秒
            printf("exit\n");
            break;
        }
    }
    
    close(fd);
    return 0;
}