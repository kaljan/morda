#ifndef BUTTONS_H
#define BUTTONS_H


#include "periph.h"


typedef enum _ButtonType {
	BTN_MENU,
	BTN_UP,
	BTN_DOWN,
	BTN_EXIT
} ButtonType;

typedef enum _ButtonSignalType {
	B_NO_SIGNAL = 0,
	B_PRESSED
} ButtonSignalTpye;

typedef struct _ButtonDescriptor {
	int BtnPin;
	ButtonType BtnType;
	GPIO_State BtnState;
	ButtonSignalTpye BtnSignal;
} ButtonDescriptor;

struct ButtonList {
	ButtonDescriptor *BtnDsc;
	struct ButtonList *next;
};

int ButtonsInit(void);

#endif BUTTONS_H
