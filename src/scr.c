#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
//#include <fcntl.h>
//#include <errno.h>
//#include <time.h>

//#include <sys/stat.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/ioctl.h>
//#include <sys/sysinfo.h>

//#include <netinet/in.h>
//#include <net/if.h>
//#include <arpa/inet.h>

#include "ssd1311.h"


extern int gpio_reset_ssd1311(void);
extern int gpio_reset_off(void);

extern int get_time(char *str);
extern int show_date(void);
extern int show_week_day(void);

extern int get_ip_addr(const char *iface, char *ipstr);

extern int show_cpu_temp(void);


extern int end_app;


typedef int (*_fn_t)(void);

struct _fn_list_t {
	_fn_t fn;
	struct _fn_list_t *next;
};


char ssd1311_str[33];

_fn_t sh_fns[] = {
	show_date,
	show_week_day,
	show_cpu_temp,
	0
};

struct _fn_list_t *sh_fn_list;


int scr_init(void)
{
	if (gpio_reset_ssd1311() != 0) {
		printf("[%s:%d] Reset SSD1311 failed\n", __func__, __LINE__);
		return -1;
	}

	if (ssd1311_init() != 0) {
		printf("[%s:%d] Init SSD1311 failed\n", __func__, __LINE__);
		return -1;
	}

	if (ssd1311_on() != 0) {
		printf("[%s:%d] Running SSD1311 failed\n", __func__, __LINE__);
		return -1;
	}

	if (ssd1311_set_string("Hello World!\0", 0) != 0) {
		printf("[%s:%d] Write text on SSD1311 failed\n", __func__, __LINE__);
		return -1;
	}
}

int scr_deinit(void)
{
	if (gpio_reset_off() != 0) {
		return -1;
	}
}

int init_fn(struct _fn_list_t **list, _fn_t *fnlst)
{
	struct _fn_list_t *top;

	if (list == 0) {
		return -1;
	}

	*list = malloc(sizeof(struct _fn_list_t));

	if (*list == 0) {
		return -1;
	}

	(*list)->fn = *fnlst;
	fnlst++;
	top = *list;


	while (*fnlst != 0) {
		(*list)->next = malloc(sizeof(struct _fn_list_t));
		*list = (*list)->next;
		(*list)->fn = *fnlst;
		fnlst++;
	}

	(*list)->next = top;
	*list = top;

	return 0;
}

int animation(void)
{
	int i;

	i = sprintf(ssd1311_str, "PID: %d", getpid());
	memset(&ssd1311_str[i], ' ', 32 - i);
	ssd1311_set_string(ssd1311_str, 0);
	usleep(500000);

	i = 0;

	init_fn(&sh_fn_list, sh_fns);

	while (1) {
		usleep(500000);
		if (end_app == 1) {
			break;
		}

		get_time(ssd1311_str);

		(sh_fn_list->fn)();

		ssd1311_set_string(ssd1311_str, 0);


		i++;
		if (i > 60) {
			i = 0;
			sh_fn_list = sh_fn_list->next;
		}
	}

	sprintf(ssd1311_str, "EXIT");
	memset(&ssd1311_str[4], ' ', 28);
	ssd1311_set_string(ssd1311_str, 0);

	return 0;
}
