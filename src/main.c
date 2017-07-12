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

int main(int argc, char **argv)
{

//	ButtonsInit(btnss);
//	return 0;
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
