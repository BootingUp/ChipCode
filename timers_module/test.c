/**================================================================
*   Copyright (C) 2020  ZhangYiFei. All rights reserved.
*   
*   文件名称：test.c
*   创 建 者：ZhangYiFei
*   创建日期：2020年01月14日
*   描    述：
*
================================================================**/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
  #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>


#include "mult_time.h"

int main(int argc, const char *argv[])
{
	int ret = 0;
	int timer_fd = open("/dev/mult_time", O_RDWR);

	mult_time_t timer = { 0 };
    timer.time_id = 3;
    timer.timeout_s = 4;

	ret = ioctl(timer_fd, MULT_TIME_START, (unsigned long)&timer);
	printf("%d\n", ret);
	
	while (1);

	return 0;
}
