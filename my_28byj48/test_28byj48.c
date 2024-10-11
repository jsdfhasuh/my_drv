#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

int fd;
int status[2];
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
		status[0] = 1;
        status[1] = 10;
		write(fd, &status, sizeof(status));
	}
	else if (0 == strcmp(argv[2], "off"))
	{
		status[0] = 0;
		write(fd, &status, sizeof(status));
	}
    close(fd);
    return 0;
}