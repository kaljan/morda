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

typedef int (*ButtonCallback)(void* context);

typedef struct _ButtonDescriptor {
	int buttonPin;
	ButtonType buttonType;
	GPIO_State buttonsState;
	ButtonSignalTpye buttonSignal;
	int buttonPressCounter;
//	typedef int (*ButtonCallback)(void* context);
	ButtonCallback bCallback;
} ButtonDescriptor;

struct ButtonList {
	ButtonDescriptor *buttonDescriptor;
	struct ButtonList *next;
};

typedef struct _Button {
	int bPin;
	ButtonType bType;
	ButtonCallback bCallback;
} Button;

int ButtonsInit(Button *button);
int ButtonProcessStart(void);
int ButtonProcessStop(void);
int ButtonsDeinit(void);

#endif // BUTTONS_H
