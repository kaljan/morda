#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "ssd1311.h"

#include "si.h"

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


char lcd_str[33];

_fn_t sh_fns[] = {
	show_date,
	show_week_day,
	show_cpu_temp,
	0
};

struct _fn_list_t *sh_fn_list;


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

	if (ssd1311_init() != 0) {
		printf("[%s:%d] SSD1311 init failed\n", __func__, __LINE__);
		ssd1311_deinit();
		return -1;
	}

	i = sprintf(lcd_str, "PID: %d", getpid());
	memset(&lcd_str[i], ' ', 32 - i);
	ssd1311_set_string(lcd_str, 0);
	usleep(500000);

	i = 0;

	init_fn(&sh_fn_list, sh_fns);

	while (1) {
		usleep(500000);
		if (end_app == 1) {
			break;
		}

		get_time(lcd_str);

		(sh_fn_list->fn)();

		ssd1311_set_string(lcd_str, 0);

		i++;
		if (i > 60) {
			i = 0;
			sh_fn_list = sh_fn_list->next;
		}
	}

	sprintf(lcd_str, "EXIT");
	memset(&lcd_str[4], ' ', 28);
	ssd1311_set_string(lcd_str, 0);

	if (ssd1311_deinit() != 0) {
		return -1;
	}

	return 0;
}
