#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static int gpio = -1;

static void fadeInOut(int step,int max,int delay,int count)
{
	int i,j,k;
	for(i=step,j=step,k=0;k<count;) {
		write(gpio,"0",1);
		if (i > 0) {
			usleep(i);
		}
		write(gpio,"1",1);
		usleep(max*2);
		if (i > max) {
			j = -step;
		} else if (i <= 0) {
			usleep(delay);
			++k;
			j = step;
		}
		i += j;
	}
}

int main(int argc,char **argv)
{
	gpio = open("/dev/shm/led-gpio-value",O_WRONLY);
	if (gpio <= 0) {
		return -1;
	}

	if ((argc <= 1)||(!strcmp("sos",argv[1]))) {
		fadeInOut(15,1000,100000,3);
		usleep(350000);
		fadeInOut(8,1000,100000,3);
		usleep(350000);
		fadeInOut(15,1000,100000,3);
	} else if (!strcmp("reset",argv[1])) {
		fadeInOut(200,1000,100000,25);
	} else if (!strcmp("boot",argv[1])) {
		fadeInOut(2,1000,100000,1);
	}

	return 0;
}
