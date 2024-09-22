
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>


/*
 * ./ledtest /dev/100ask_led0 on
 * ./ledtest /dev/100ask_led0 off
 */
int main(int argc, char **argv)
{
	int fd;
	char status;
	struct pollfd fds[1];
	int ret;
	int val;
	int timeout_ms = 500000;
	/* 1. 判断参数 */
	if (argc != 3) 
	{
		printf("Usage: %s <dev> <on | off>\n", argv[0]);
		return -1;
	}

	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	/* 3. 写文件 */
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
	else if (0 == strcmp(argv[2], "read"))
	{
		read(fd, &status, 1);
		printf("app status = %d\n", status);
	}
	else if (0 == strcmp(argv[2], "poll"))
	{
		fds[0].fd = fd;
		fds[0].events = POLLIN;
		ret = poll(fds, 1, timeout_ms);
		if ((ret == 1) && (fds[0].revents & POLLIN))
		{
			read(fd, &val, 4);
			printf("get button : 0x%x\n", val);
		}
		else
		{
			printf("timeout\n");
		}

	}
	else
	{
		printf("Usage: %s <dev> <on | off>\n", argv[0]);
		return -1;
	}
	
	close(fd);
	
	return 0;
}


