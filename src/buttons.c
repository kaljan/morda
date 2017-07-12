#include "buttons.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>


struct ButtonList *buttonList;
ButtonDescriptor *buttonDescriptor;
pthread_t buttonThread;
int buttonProcessExitFlag;


int ButtonHandler(ButtonDescriptor *button)
{
	GPIO_State state;
	if (button == 0) {
		printf("[%s:%d] Btton Handle error;\n"
			,__FUNCTION__, __LINE__);

		return -1;
	}

	state = GPIO_Debounce(button->buttonPin);

	if (button->buttonsState == GPIO_HIGH) {
		if (state == GPIO_LOW) {
			button->buttonsState = GPIO_LOW;
		}
		return 0;
	}

	if (state == GPIO_HIGH) {
		button->buttonsState = GPIO_HIGH;
		button->buttonSignal = B_PRESSED;
		return 0;
	}
	return 0;
}

ButtonDescriptor *getButtonDescriptor(struct ButtonList *list)
{
	ButtonDescriptor *dsc = NULL;

	while (list) {
		if (ButtonHandler(list->buttonDescriptor) != 0) {
			printf("[%s:%d] Get button descriptor failed Failed;\n"
				,__FUNCTION__, __LINE__);
			break;
		}

		if (list->buttonDescriptor->buttonsState == GPIO_LOW) {
			dsc = list->buttonDescriptor;
			break;
		}

		list = list->next;
	}

	return dsc;
}

void *ButtonThread(void *arg)
{
	while (1) {
		if (buttonProcessExitFlag != 0) {
			buttonDescriptor = NULL;
			buttonProcessExitFlag = 0;
			break;
		}

		if (buttonDescriptor == NULL) {
			buttonDescriptor = getButtonDescriptor(buttonList);
			continue;
		}

		if (ButtonHandler(buttonDescriptor) != 0) {
			printf("[%s:%d] Failed;\n"
				,__FUNCTION__, __LINE__);
			continue;
		}

		if (buttonDescriptor->buttonSignal == B_NO_SIGNAL) {
			continue;
		}

		buttonDescriptor->buttonSignal = B_NO_SIGNAL;

		if (buttonDescriptor->bCallback != NULL) {
			(buttonDescriptor->bCallback)(arg);
			buttonDescriptor = NULL;
		}

		// TODO: Добавить обработку отсутствия колбэка

		buttonDescriptor = NULL;
	}
	return arg;
}


int CreateButton(ButtonDescriptor **button, int pin, ButtonType btype, ButtonCallback callback)
{
	if ((*button = malloc(sizeof(ButtonDescriptor))) == NULL) {
		printf("[%s:%d] Create button failed: Memory allocate error;\n"
			, __FUNCTION__, __LINE__);
		return -1;
	}

	(*button)->buttonPin = pin;
	(*button)->buttonType = btype;
	(*button)->buttonsState = GPIO_HIGH;
	(*button)->buttonSignal = B_NO_SIGNAL;
	(*button)->buttonPressCounter = 0;
	(*button)->bCallback = callback;

	if (GPIO_Export(pin) != 0) {
		printf("[%s:%d] Create button failed: GPIO error;\n"
			, __FUNCTION__, __LINE__);
		return -1;
	}

	if (GPIO_SetDirection(pin, "in") != 0) {
		printf("[%s:%d] Create button failed: GPIO error;\n"
			, __FUNCTION__, __LINE__);

		return -1;
	}

	return 0;
}

int AddButtonToList(struct ButtonList **list, int pin, ButtonType btype, ButtonCallback callback)
{
	struct ButtonList *first = NULL;

	if (list == NULL) {
		printf("[%s:%d] Create button list failed: NULL Pointer;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	if (*list == 0) { // Список не создан
		// Создаём список
		if ((*list = malloc(sizeof(struct ButtonList))) == NULL) {
			printf("[%s:%d] Create button list failed: Memory allocate error;\n"
				,__FUNCTION__, __LINE__);
			return -1;
		}
	} else {
		first = *list;
		while ((*list)->next) {
			*list = (*list)->next;
		}

		if (((*list)->next = malloc(sizeof(struct ButtonList))) == NULL) {
			printf("[%s:%d] Create button list failed: Memory allocate error;\n"
				,__FUNCTION__, __LINE__);
			return -1;
		}
		*list = (*list)->next;
	}

	if (CreateButton(&((*list)->buttonDescriptor), pin, btype, callback) != 0) {
		return -1;
	}

	if (first != NULL) {
		*list = first;
	}

	return 0;
}

int ButtonProcessStart(void)
{
	int ret;
	if ((ret = pthread_create(&buttonThread, NULL, ButtonThread, NULL)) != 0) {
		printf("[%s:%d] Creating button thread failed: %s\n"
			, __func__, __LINE__, strerror(ret));
		return -1;
	}

	return 0;
}

int ButtonsInit(Button *button)
{
	buttonList = NULL;
	buttonDescriptor = NULL;
	buttonProcessExitFlag = 0;

	while (button->bPin != -1) {
		if (AddButtonToList(&buttonList, button->bPin,
				button->bType, button->bCallback) != 0) {
			printf("[%s:%d] ButtonsInit failed;\n"
				,__FUNCTION__, __LINE__);
			return -1;
		}
		button++;
		printf("\n");
	}

	return 0;
}

int ButtonProcessStop(void)
{
	buttonProcessExitFlag = 1;
	while (buttonProcessExitFlag == 1);
	return 0;
}

int DestroyButton(ButtonDescriptor ** button)
{
	if (button == 0) {
		printf("[%s:%d] Destroy button failed;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	if (*button == 0) {
		printf("[%s:%d] Descriptor already destroyed;\n"
			,__FUNCTION__, __LINE__);
		return 0;
	}

	if (GPIO_Unexport((*button)->buttonPin) != 0) {
		printf("[%s:%d] GPIO unexport failed;\n"
			,__FUNCTION__, __LINE__);
	}

	free(*button);
	*button = 0;
	return 0;
}

int RemoveButtonFromList(struct ButtonList **list)
{
	struct ButtonList *first, *prev;

	if (list == 0) {
		return -1;
	}

	if (*list == 0) {
		return 0;
	}

	first = *list;
	prev = first;

	while ((*list)->next) {
		prev = (*list);
		(*list) = (*list)->next;
	}

	DestroyButton(&((*list)->buttonDescriptor));

	if ((*list) == first) {
		free(*list);
		*list = 0;
		return 0;
	}

	*list = first;

	free(prev->next);
	prev->next = NULL;

	return 0;
}

int ButtonsDeinit(void)
{
	while (buttonList) {
		if (RemoveButtonFromList(&buttonList) != 0) {
			printf("[%s:%d] Pop failed;\n"
				,__FUNCTION__, __LINE__);
			return -1;
		}
		printf("\n");
	}

	return 0;
}
