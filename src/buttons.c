#include "buttons.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>


struct ButtonList *buttonList;
ButtonDescriptor *bdsc;
pthread_t buttonThread;


int ButtonHandler(ButtonDescriptor *button)
{
	GPIO_State state;
	if (button == 0) {
		printf("[%s:%d] Btton Handle error;\n"
			,__FUNCTION__, __LINE__);

		return -1;
	}

	state = GPIO_Debounce(button->BtnPin);

	if (button->BtnState == GPIO_HIGH) {
		if (state == GPIO_LOW) {
			button->BtnState = GPIO_LOW;
		}
		return 0;
	}

	if (state == GPIO_HIGH) {
		button->BtnState = GPIO_HIGH;
		button->BtnSignal = B_PRESSED;
		return 0;
	}
	return 0;
}

ButtonDescriptor *getButtonDescriptor(struct ButtonList *list)
{
	ButtonDescriptor *dsc = NULL;

	while (list) {
		if (ButtonHandler(list->BtnDsc) != 0) {
			printf("[%s:%d] Failed;\n"
				,__FUNCTION__, __LINE__);
			break;
		}

		if (list->BtnDsc->BtnState == GPIO_LOW) {
			dsc = list->BtnDsc;
			break;
		}

		list = list->next;
	}

	return dsc;
}

void *ButtonThread(void *arg)
{
	if (bdsc == NULL) {
		bdsc = getButtonDescriptor(buttonList);
		return NULL;
	}

	if (ButtonHandler(bdsc) != 0) {
		printf("[%s:%d] Failed;\n"
			,__FUNCTION__, __LINE__);
		return NULL;
	}

	switch (bdsc->BtnType) {
		case BTN_MENU:
			printf("[%s:%d] Button \"MENU\" pressed (%d);\n"
				,__FUNCTION__, __LINE__, bdsc->BtnPrsCnt);
			break;

		case BTN_UP:
			printf("[%s:%d] Button \"UP\" pressed (%d);\n"
				,__FUNCTION__, __LINE__, bdsc->BtnPrsCnt);
			break;

		case BTN_DOWN:
			printf("[%s:%d] Button \"DOWN\" pressed (%d);\n"
				,__FUNCTION__, __LINE__, bdsc->BtnPrsCnt);

			break;

		case BTN_EXIT:
			printf("[%s:%d] Button \"EXIT\" pressed (%d);\n"
				,__FUNCTION__, __LINE__, bdsc->BtnPrsCnt);
			break;

		default:
			printf("[%s:%d] Button \"UNKNOWN\" pressed (%d);\n"
				,__FUNCTION__, __LINE__, bdsc->BtnPrsCnt);
			break;
	}
	bdsc->BtnPrsCnt++;
	return arg;
}

int InitButtonInput(int pin)
{
	if (gpio_export(pin) != 0) {
		return -1;
	}

	if (gpio_set_direction(pin, "in") != 0) {
		return -1;
	}

	return 0;
}

int CreateButton(ButtonDescriptor **bdsc, int pin, ButtonType btype)
{
	if (bdsc == NULL) {
		printf("[%s:%d] Create button failed: NULL pointer;\n"
			, __FUNCTION__, __LINE__);
		return -1;
	}

	if (*bdsc == NULL) {
		if ((*bdsc = malloc(sizeof(ButtonDescriptor))) == NULL) {
			printf("[%s:%d] Create button failed: Memory allocate error;\n"
				, __FUNCTION__, __LINE__);
			return -1;
		}
	}

	if (InitButtonInput(pin) != 0) {
		printf("[%s:%d] Create button failed: GPIO error;\n"
			, __FUNCTION__, __LINE__);
		return -1;
	}

	(*bdsc)->BtnPin = pin;
	(*bdsc)->BtnType = btype;
	(*bdsc)->BtnState = GPIO_HIGH;
	(*bdsc)->BtnSignal = B_NO_SIGNAL;
	(*bdsc)->BtnPrsCnt = 0;

	return 0;
}

int AddButtonToList(struct ButtonList **blist, int pin, ButtonType btype)
{
	struct ButtonList **bptr;

	if (blist == NULL) {
		printf("[%s:%d] Create button list failed: NULL Pointer;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	bptr = blist;
	while (*bptr) {
		*bptr = (*bptr)->next;
	}

	if ((*bptr = malloc(sizeof(struct ButtonList))) == NULL) {
		printf("[%s:%d] Create button list failed: Memory allocate error;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}
	(*bptr)->BtnDsc = NULL;
	(*bptr)->next = NULL;


	if (CreateButton(&((*blist)->BtnDsc), pin, btype) != 0) {
		printf("[%s:%d] Create button list failed: Create button error;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int ButtonsInit(void)
{
	int ret;

	buttonList = NULL;
	bdsc = NULL;

	if (AddButtonToList(&buttonList, 25, BTN_MENU) != 0) {
		printf("[%s:%d] ButtonsInit failed;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	if (AddButtonToList(&buttonList, 22, BTN_UP) != 0) {
		printf("[%s:%d] ButtonsInit failed;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	if (AddButtonToList(&buttonList, 27, BTN_DOWN) != 0) {
		printf("[%s:%d] ButtonsInit failed;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	if (AddButtonToList(&buttonList, 17, BTN_EXIT) != 0) {
		printf("[%s:%d] ButtonsInit failed;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	if ((ret = pthread_create(&buttonThread, NULL, ButtonThread, NULL)) != 0) {
		printf("[%s:%d] Creating button thread failed: %s\n"
			, __func__, __LINE__, strerror(ret));
		return -1;
	}

	return 0;
}
