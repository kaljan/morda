#include <stdio.h>
#include <signal.h>

#include "buttons.h"

int end_app;

extern int animation(void);

int MenuButtonHandler(void *context);
int UpButtonHandler(void *context);
int DownButtonHandler(void *context);
int ExitButtonHandler(void *context);


static void catch_signal(int sig)
{
	if (sig == SIGQUIT) {
		printf("[%s:%d] Signal %d recieved\n", __FUNCTION__, __LINE__, sig);
		end_app = 1;
	}
}

Button btnss[] = {
	{25, BTN_MENU, MenuButtonHandler},
	{22, BTN_UP, UpButtonHandler},
	{27, BTN_DOWN, DownButtonHandler},
	{17, BTN_EXIT, ExitButtonHandler},
	{-1, 0, NULL}
};

int MenuButtonHandler(void *context)
{
	static int i;
	printf("[%s:%d] Button \"MENU\" bpressed (%d);\n"
		, __FUNCTION__, __LINE__, i++);
	return 0;
}

int UpButtonHandler(void *context)
{
	static int i;
	printf("[%s:%d] Button \"UP\" bpressed (%d);\n"
		, __FUNCTION__, __LINE__, i++);
	return 0;
}

int DownButtonHandler(void *context)
{
	static int i;
	printf("[%s:%d] Button \"DOWN\" bpressed (%d);\n"
		, __FUNCTION__, __LINE__, i++);
	return 0;
}

int ExitButtonHandler(void *context)
{
	static int i;
	printf("[%s:%d] Button \"EXIT\" bpressed (%d);\n"
		, __FUNCTION__, __LINE__, i++);
	return 0;
}


/*
TODO: Добавить ошибки и их обработчики.
TODO: Добавить в дисплей функцию standby
Ошибки:
NULL pointer
Bad file
.....
Другие ошибки

Процесс инициализации
1. Сначала инициализируем дисплей. если его инициализация сломалась
   то дальше работку проги продолжать безсмысленно.
2. Инициализируем кнопки. таже херня.
3. ну а дальше подумаем.

TODO: создать структуру меню

будет зацикленный двухсвязный список список.
в которому будут:
MenuDescriptor
prev
next

в описании менюхи будут
id
name
callback

*/
int main(int argc, char **argv)
{

#if 1
	ButtonsInit(btnss);

	do {
		if (ButtonProcessStart() != 0) {
			break;
		}

		printf("Press any key to continue...\n");
		getchar();

		ButtonProcessStop();

	} while (0);

	ButtonsDeinit();
#else

	end_app = 0;
	signal(SIGQUIT, catch_signal);

	animation();

	printf("Exit\n");
#endif
	return 0;
}
