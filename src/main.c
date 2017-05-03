#include <stdio.h>
#include <signal.h>


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
	end_app = 0;
	signal(SIGQUIT, catch_signal);

	animation();

	printf("Exit\n");

	return 0;
}
