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
	if (buttonDescriptor == NULL) {
		buttonDescriptor = getButtonDescriptor(buttonList);
		return NULL;
	}

	if (ButtonHandler(buttonDescriptor) != 0) {
		printf("[%s:%d] Failed;\n"
			,__FUNCTION__, __LINE__);
		return NULL;
	}

	switch (buttonDescriptor->BtnType) {
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
	printf("[%s:%d] Creating button\n"
		,__FUNCTION__, __LINE__);

	if (bdsc == NULL) {
		printf("[%s:%d] Create button failed: NULL pointer;\n"
			, __FUNCTION__, __LINE__);
		return -1;
	}

	printf("[%s:%d] Allocate button descriptor\n"
		,__FUNCTION__, __LINE__);

	if (*bdsc == NULL) {
		if ((*bdsc = malloc(sizeof(ButtonDescriptor))) == NULL) {
			printf("[%s:%d] Create button failed: Memory allocate error;\n"
				, __FUNCTION__, __LINE__);
			return -1;
		}
	}

	printf("[%s:%d] Init GPIO input\n"
		,__FUNCTION__, __LINE__);

//	if (InitButtonInput(pin) != 0) {
//		printf("[%s:%d] Create button failed: GPIO error;\n"
//			, __FUNCTION__, __LINE__);
//		return -1;
//	}

	printf("[%s:%d] Init gpio descriptor\n"
		,__FUNCTION__, __LINE__);

	(*bdsc)->BtnPin = pin;
	(*bdsc)->BtnType = btype;
	(*bdsc)->BtnState = GPIO_HIGH;
	(*bdsc)->BtnSignal = B_NO_SIGNAL;
	(*bdsc)->BtnPrsCnt = 0;

	return 0;
}

int CreateItem(struct ButtonList **blist, int pin, ButtonType btype)
{
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

int AddButtonToList(struct ButtonList **blist, int pin, ButtonType btype)
{
	struct ButtonList *first;

	if (blist == NULL) {
		printf("[%s:%d] Create button list failed: NULL Pointer;\n"
			,__FUNCTION__, __LINE__);
		return -1;
	}

	if (*blist == 0) { // Список не создан
		printf("[%s:%d] Creating first element\n"
			,__FUNCTION__, __LINE__);
		// Создаём список
		if (CreateItem(blist, pin, btype) != 0) {
			return -1;
		}
		return 0;
	}

	printf("[%s:%d] Creating next element\n"
		,__FUNCTION__, __LINE__);

	first = *blist;
	while ((*blist)->next) {
		printf("[%s:%d] Go to last element [%p]\n"
			,__FUNCTION__, __LINE__, *blist);
		*blist = (*blist)->next;
	}

	if (CreateItem(&((*blist)->next), pin, btype) != 0) {
		return -1;
	}

	*blist = first;
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

	getButtonType(btn_type_str, button->BtnType);
	getGPIO_State(btn_state_str, button->BtnState);
	geButtonSignalTpye(btn_signal_str, button->BtnSignal);

	sprintf(str,
	"    Pin   : %d\n"
	"    Type  : %s\n"
	"    State : %s\n"
	"    Signal: %s\n"
	"    Count : %d\n"
	, button->BtnPin
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
		printf("  ButtonDescriptor [%p]\n", btnlist->BtnDsc);
		ButtonDescriptorToStr(btn_dsc_str, btnlist->BtnDsc);
		printf("%s", btn_dsc_str);
		btnlist = btnlist->next;
		printf("\nnext [%p]\n", btnlist);
	}
}

int ButtonsInit(void)
{
//	int ret;
	struct _button_t *btnptr = buttons;
	char str[16];

	buttonList = NULL;
	buttonDescriptor = NULL;

	printf("[%s:%d] Start init buttons;\n"
		,__FUNCTION__, __LINE__);

	while (btnptr->BtnPin != -1) {

		getButtonType(str, btnptr->BtnType);
		printf("[%s:%d] Adding %s on GPIO%d;\n"
			,__FUNCTION__, __LINE__, str, btnptr->BtnPin);


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
#if 0
	if ((ret = pthread_create(&buttonThread, NULL, ButtonThread, NULL)) != 0) {
		printf("[%s:%d] Creating button thread failed: %s\n"
			, __func__, __LINE__, strerror(ret));
		return -1;
	}
#endif
	return 0;
}

//	if (AddButtonToList(&buttonList, 25, BTN_MENU) != 0) {
//		printf("[%s:%d] ButtonsInit failed;\n"
//			,__FUNCTION__, __LINE__);
//		return -1;
//	}

//	if (AddButtonToList(&buttonList, 22, BTN_UP) != 0) {
//		printf("[%s:%d] ButtonsInit failed;\n"
//			,__FUNCTION__, __LINE__);
//		return -1;
//	}

//	if (AddButtonToList(&buttonList, 27, BTN_DOWN) != 0) {
//		printf("[%s:%d] ButtonsInit failed;\n"
//			,__FUNCTION__, __LINE__);
//		return -1;
//	}

//	if (AddButtonToList(&buttonList, 17, BTN_EXIT) != 0) {
//		printf("[%s:%d] ButtonsInit failed;\n"
//			,__FUNCTION__, __LINE__);
//		return -1;
//	}
