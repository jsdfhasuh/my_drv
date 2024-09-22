#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

int fd;
int status;
int i;
unsigned char datas[5];

static void sig_func(int sig)
{
    // 读取设备状态
    printf("begin\n");
    read(fd, datas, sizeof(datas));
    for (i=0;i<5;i++)
        printf("value is %d\n",datas[i]);
    printf("exit\n");
}

int main(int argc, char **argv)
{

    int flags;
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
    else if ( 0 == strcmp(argv[2],"async"))
    {
        signal(SIGIO,sig_func);
        fcntl(fd,F_SETOWN,getpid());
        flags = fcntl(fd,F_GETFL);
        fcntl(fd,F_SETFL,flags|FASYNC);
        while (1)
        {
            printf("wait data");
            sleep(1);
        }
    }
    else if ( 0 == strcmp(argv[2],"noasync"))
    {
        // 获取当前文件描述符标志
        int flags = fcntl(fd, F_GETFL);
        if (flags == -1) {
            perror("fcntl");
            close(fd);
        }

        // 移除 FASYNC 标志
        flags &= ~FASYNC;

        // 设置更新后的标志
        if (fcntl(fd, F_SETFL, flags) == -1) {
            perror("fcntl");
            close(fd);
        }

        printf("异步I/O功能已关闭\n");

        close(fd);
        return 0;
        }
    close(fd);
    return 0;
}