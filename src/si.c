#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>

#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "si.h"


extern char lcd_str[33];


time_t t;
struct tm *tmptr;
struct sysinfo si;

int si_update(void)
{
	 if (sysinfo(&si) < 0) {
		 printf("Can\'t get sysinfo: %s\n", strerror(errno));
		 return -1;
	 }

	return 0;
}

int get_uptime(char *str, uint32_t time)
{
	int mins = 0, secs = 0, hours = 0;

	if (str == 0) {
		return -1;
	}

	do {
		secs = time % 60;
		time /= 60;
		if (time == 0) {
			break;
		}

		mins = time % 60;
		time /= 60;
		if (time == 0) {
			break;
		}

		hours = time % 24;
		time /= 24;
		if (time == 0) {
		}


	} while(0);

	sprintf(str, "%d d. %02d:%02d:%02d", time, hours, mins, secs);

	return 0;
}

int show_uptime(void)
{
	si_update();
	get_uptime(&lcd_str[16], si.uptime);
	return 0;
}



int get_cpu_temp(void)
{
	static const char cpu_temperature_path[] =
		"/sys/class/thermal/thermal_zone0/temp\0";

	int fd;
	char cpu_tmp_str[8];
	int cpu_temp;
	memset(cpu_tmp_str, 0, 8);

	if ((fd = open(cpu_temperature_path, O_RDONLY)) < 0) {
		printf("[%s:%d] Can\'t open %s: %s\n"
			, __func__, __LINE__, cpu_temperature_path, strerror(errno));
		return -1;
	}

	if (read(fd, cpu_tmp_str, 8) < 0) {
		printf("[%s:%d] Can\'t read %s: %s\n"
			, __func__, __LINE__, cpu_temperature_path, strerror(errno));
		close(fd);
		return -1;
	}

	close(fd);
	cpu_temp = atoi(cpu_tmp_str);

	return cpu_temp;
}

int show_cpu_temp(void)
{
	int cpu_temperature;
	cpu_temperature = get_cpu_temp();

	if (cpu_temperature > 0) {
		cpu_temperature /= 100;
		sprintf(&lcd_str[16], "CPU: %2d.%1d \'C    "
			, cpu_temperature / 10, cpu_temperature % 10);
	}
	return 0;
}

int get_time(char *str)
{
	if (str == 0) {
		return -1;
	}
	t = time(NULL);
	tmptr = localtime(&t);
	strftime(str, 9, "%T ", tmptr);
	memset(&str[8], ' ', 24);
	return 0;
}

int show_date(void)
{
	char str[16];
	int l;

	strftime(str, 13, "%d-%b-%Y ", tmptr);
	l = strlen(str);
	memset(&str[l], ' ', 16 - l);
	strcpy(&lcd_str[16], str);
	return 0;
}

int show_week_day(void)
{
	char str[16];
	int l;

	strftime(str, 16, "%A ", tmptr);
	l = strlen(str);
	memset(&str[l], ' ', 16 - l);
	strcpy(&lcd_str[16], str);
	return 0;
}

int get_ip_addr(const char *iface, char *ipstr)
{
	int fd, rv;
	struct ifreq ifr;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Failed to open socket: %s\n", strerror(errno));
		return -1;
	}

	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name , iface , IFNAMSIZ - 1);

	if ((rv = ioctl(fd, SIOCGIFADDR, &ifr)) == -1) {
		printf("IOCTL error: %s\n", strerror(errno));
		return -1;
	}

	close(fd);

	sprintf(ipstr, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	return 0;
}
