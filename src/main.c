#include <stdio.h>
#include <signal.h>

#include "buttons.h"

int end_app;

extern int animation(void);

static void catch_signal(int sig)
{
	if (sig == SIGQUIT) {
		printf("[%s:%d] Signal %d recieved\n", __FUNCTION__, __LINE__, sig);
		end_app = 1;
	}
}

int main(int argc, char **argv)
{
#if 1
	ButtonsInit(NULL);

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
