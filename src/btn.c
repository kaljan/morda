#include <stdio.h>
#include <string.h>

#include <pthread.h>

#include "periph.h"


#define NUMBER_BTNS	4


int btn_pins[NUMBER_BTNS] = {25, 22, 27, 17};


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

static int btn_pressed(int btn)
{
	int btn_value;

	if ((btn_value = gpio_get_value(btn)) != 0) {
		return -1;
	}

	if (btn_value == 1) {
		return 0;
	}

	while (btn_value == 0) {
		if ((btn_value = gpio_get_value(btn)) != 0) {
			return -1;
		}
	}

	return 1;
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

void *btn_thread(void *arg)
{
	return arg;
}

int btn_init(void)
{
	int i;
	for (i = 0; i < NUMBER_BTNS; i++) {
		if (btn_cfg_in(btn_pins[i]) != 0) {
			printf("[%s:%d] BTN config failed\n", __func__, __LINE__);
			return -1;
		}
	}

	return 0;
}

int btn_deinit(void)
{
	int i;
	for (i = 0; i < NUMBER_BTNS; i++) {
		if (gpio_unexport(btn_pins[i]) != 0) {
			printf("[%s:%d] BTN unexport failed\n", __func__, __LINE__);
		}
	}
	return 0;
}
