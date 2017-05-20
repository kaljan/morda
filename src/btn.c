#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#include "periph.h"


#define NUMBER_BTNS	4


typedef struct _btn_t {
	int id;
	int pin;
	int value;
	int event;
} btn_t;

btn_t btn[NUMBER_BTNS] = {
	{1, 25, 1, 0},
	{2, 22, 1, 0},
	{3, 27, 1, 0},
	{4, 17, 1, 0}
};

int btn_pins[NUMBER_BTNS] = {25, 22, 27, 17};

int btn_state[NUMBER_BTNS] = {0,0,0,0};

pthread_t _btn_thread;

static int btn_cfg_in(int pin)
{
	if (gpio_export(pin) != 0) {
		return -1;
	}

	if (gpio_set_direction(pin, "in") != 0) {
		return -1;
	}

	return 0;
}

int btn_debounce(int pin)
{
	int debounce_c = 128;
	int ival, oval;

	while (1) {
		ival = gpio_get_value(pin);
		if (ival == 0) {
			debounce_c++;
			if (debounce_c > 256) {
				oval = 0;
				break;
			}
			continue;
		}
		debounce_c--;
		if (debounce_c < 1) {
			oval = 1;
			break;
		}
	}
	return oval;
}

int btn_handler(btn_t *btn)
{

	if (btn->value == 1) {
		btn->value = btn_debounce(btn->pin);
		return 0;
	}

	btn->value = btn_debounce(btn->pin);
	if (btn->value == 0) {
		btn->value = btn_debounce(btn->pin);
		return 0;
	}

	btn->event = 1;

	return 0;
}

void *btn_thread(void *arg)
{
	while (1) {
		int i = 0;
		for (i = 0; i < NUMBER_BTNS; i++) {
			btn_handler(&btn[i]);
			if (btn[i].event == 1) {
				btn[i].event = 0;
				printf("[%s:%d] BUTTON%d pressed\n"
					, __func__, __LINE__, btn[i].id);
			}
		}

	}
	printf("[%s:%d] Exit\n", __func__, __LINE__);
	return arg;
}

int btn_init(void)
{
	int i, ret;
	for (i = 0; i < NUMBER_BTNS; i++) {
		if (btn_cfg_in(btn[i].pin) != 0) {
			printf("[%s:%d] BTN config failed\n", __func__, __LINE__);
			return -1;
		}
	}

	if ((ret = pthread_create(&_btn_thread, NULL, btn_thread, NULL)) != 0) {
		printf("[%s:%d] Creating button thread failed: %s\n"
			, __func__, __LINE__, strerror(ret));
		return -1;
	}


	return 0;
}

int btn_deinit(void)
{
	int i;
	pthread_cancel(_btn_thread);
	for (i = 0; i < NUMBER_BTNS; i++) {
		if (gpio_unexport(btn_pins[i]) != 0) {
			printf("[%s:%d] BTN unexport failed\n", __func__, __LINE__);
		}
	}
	return 0;
}

int btn_test(void)
{
	int i;
	int btn_state[NUMBER_BTNS];

	for (i = 0; i < NUMBER_BTNS; i++) {
		btn_state[i] = gpio_get_value(btn_pins[i]);
	}

	printf(
		"BUTTON1: %d\n"
		"BUTTON2: %d\n"
		"BUTTON3: %d\n"
		"BUTTON4: %d\n"
		"\n"
		, btn_state[0]
		, btn_state[1]
		, btn_state[2]
		, btn_state[3]
	);

	return 0;
}
