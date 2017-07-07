#include "buttons.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

	return 0;
}

int AddButtonToList(struct ButtonList **blist, int pin, ButtonType btype)
{
	struct ButtonList *bptr;

	if (blist == NULL) {
		printf("[%s:%d] Create button list failed: NULL Pointer;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	bptr = *blist;
	while (bptr) {
		bptr = bptr->next;
	}

	if ((*blist = malloc(sizeof(struct ButtonList))) == NULL) {
		printf("[%s:%d] Create button list failed: Memory allocate error;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}
	(*blist)->BtnDsc = NULL;
	(*blist)->next = NULL;


	if (CreateButton(&((*blist)->BtnDsc), pin, btype) != 0) {
		printf("[%s:%d] Create button list failed: Create button error;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int ButtonsInit(void)
{
	return 0;
}
