#include "buttons.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>


//#define DBG_HEADER	"\e[1m[\e[0;32m%s\e[0;1m:\e[0;32m%d\e[0;1m]"
//#define DBG_TERM	"\e[0m\n"


struct ButtonList *buttonList;
ButtonDescriptor *buttonDescriptor;
pthread_t buttonThread;

struct _button_t {
	int BtnPin;
	ButtonType BtnType;
} buttons[] = {
	{25, BTN_MENU},
	{22, BTN_UP},
	{27, BTN_DOWN},
	{17, BTN_EXIT},
	{-1, 0}
};


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
			printf("[%s:%d] Failed;\n"
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
	if (buttonDescriptor == NULL) {
		buttonDescriptor = getButtonDescriptor(buttonList);
		return NULL;
	}

	if (ButtonHandler(buttonDescriptor) != 0) {
		printf("[%s:%d] Failed;\n"
			,__FUNCTION__, __LINE__);
		return NULL;
	}

	switch (buttonDescriptor->buttonType) {
		case BTN_MENU:
			printf("[%s:%d] Button \"MENU\" pressed (%d);\n"
				,__FUNCTION__, __LINE__, buttonDescriptor->BtnPrsCnt);
			break;

		case BTN_UP:
			printf("[%s:%d] Button \"UP\" pressed (%d);\n"
				,__FUNCTION__, __LINE__, buttonDescriptor->BtnPrsCnt);
			break;

		case BTN_DOWN:
			printf("[%s:%d] Button \"DOWN\" pressed (%d);\n"
				,__FUNCTION__, __LINE__, buttonDescriptor->BtnPrsCnt);

			break;

		case BTN_EXIT:
			printf("[%s:%d] Button \"EXIT\" pressed (%d);\n"
				,__FUNCTION__, __LINE__, buttonDescriptor->BtnPrsCnt);
			break;

		default:
			printf("[%s:%d] Button \"UNKNOWN\" pressed (%d);\n"
				,__FUNCTION__, __LINE__, buttonDescriptor->BtnPrsCnt);
			break;
	}
	buttonDescriptor->BtnPrsCnt++;
	return arg;
}

int InitButtonInput(int pin)
{
	if (GPIO_Export(pin) != 0) {
		return -1;
	}

	if (GPIO_SetDirection(pin, "in") != 0) {
		return -1;
	}

	return 0;
}

void getButtonType(char *str, ButtonType btype)
{
	switch (btype) {
	case BTN_MENU:
		sprintf(str, "BTN_MENU");
		break;

	case BTN_UP:
		sprintf(str, "BTN_UP");
		break;

	case BTN_DOWN:
		sprintf(str, "BTN_DOWN");
		break;

	case BTN_EXIT:
		sprintf(str, "BTN_EXIT");
		break;

	default:
		sprintf(str, "UNKNOWN");
		break;
	}
}

void getGPIO_State(char *str, ButtonType btype)
{
	switch (btype) {
	case GPIO_LOW:
		sprintf(str, "GPIO_LOW");
		break;

	case GPIO_HIGH:
		sprintf(str, "GPIO_HIGH");
		break;

	default:
		sprintf(str, "UNKNOWN");
		break;
	}
}

void geButtonSignalTpye(char *str, ButtonType btype)
{
	switch (btype) {
	case B_NO_SIGNAL:
		sprintf(str, "B_NO_SIGNAL");
		break;

	case B_PRESSED:
		sprintf(str, "B_PRESSED");
		break;

	default:
		sprintf(str, "UNKNOWN");
		break;
	}
}

void ButtonDescriptorToStr(char *str, ButtonDescriptor *button)
{
	char btn_type_str[16];
	char btn_state_str[16];
	char btn_signal_str[16];

	getButtonType(btn_type_str, button->buttonType);
	getGPIO_State(btn_state_str, button->buttonsState);
	geButtonSignalTpye(btn_signal_str, button->buttonSignal);

	sprintf(str,
	"    Pin   : %d\n"
	"    Type  : %s\n"
	"    State : %s\n"
	"    Signal: %s\n"
	"    Count : %d\n"
	, button->buttonPin
	, btn_type_str
	, btn_state_str
	, btn_signal_str
	, button->BtnPrsCnt
	);

}

void printButtonList(struct ButtonList *btnlist)
{
	char btn_dsc_str[256];

	printf("\nbuttonList [%p]\n", btnlist);

	while (btnlist) {
		printf("  ButtonDescriptor [%p]\n", btnlist->buttonDescriptor);
		ButtonDescriptorToStr(btn_dsc_str, btnlist->buttonDescriptor);
		printf("%s", btn_dsc_str);
		btnlist = btnlist->next;
		printf("\nnext [%p]\n", btnlist);
	}
}

int CreateButton(ButtonDescriptor **button, int pin, ButtonType btype)
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
	(*button)->BtnPrsCnt = 0;

	if (InitButtonInput(pin) != 0) {
		printf("[%s:%d] Create button failed: GPIO error;\n"
			, __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int AddButtonToList(struct ButtonList **list, int pin, ButtonType btype)
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

	if (CreateButton(&((*list)->buttonDescriptor), pin, btype) != 0) {
		return -1;
	}

	if (first != NULL) {
		*list = first;
	}

	return 0;
}

int ButtonsInit(Button *button)
{
	struct _button_t *btnptr = buttons;
	char str[16];

	buttonList = NULL;
	buttonDescriptor = NULL;

	while (btnptr->BtnPin != -1) {

		getButtonType(str, btnptr->BtnType);

		if (AddButtonToList(&buttonList, btnptr->BtnPin,
				btnptr->BtnType) != 0) {
			printf("[%s:%d] ButtonsInit failed;\n"
				,__FUNCTION__, __LINE__);
			return -1;
		}
		btnptr++;
		printf("\n");
	}
	printButtonList(buttonList);

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

int ButtonProcessStop(void)
{
	return 0;
}

int DestroyButton(ButtonDescriptor ** button)
{
	if (button == 0) {
		printf("[%s:%d] GPIO unexport failed;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	if (*button == 0) {
		printf("[%s:%d] GPIO unexport failed;\n"
			,__FUNCTION__, __LINE__);
		return 0;
	}

	printf("[%s:%d] GPIO%d unexport;\n"
		,__FUNCTION__, __LINE__, (*button)->buttonPin);

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
		printf("[%s:%d] Go to last element;\n"
			,__FUNCTION__, __LINE__);
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
	int i = 0;

	while (buttonList) {
		printf("\nRemove item %d\n", i++);
		if (RemoveButtonFromList(&buttonList) != 0) {
			printf("[%s:%d] Pop failed;\n"
				,__FUNCTION__, __LINE__);
			return -1;
		}
	}

	return 0;
}
