#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "periph.h"

int gpio_write(const char *gpio_path, const char *value)
{
	int fd;

	if ((fd = open(gpio_path, O_WRONLY)) < 0) {
		printf("[%s:%d] Opening \'%s\' failed: %s\n"
			, __func__, __LINE__, gpio_path, strerror(errno));
		return -1;
	}

	if (write(fd, value, strlen(value)) < 0) {
		printf("[%s:%d] Write to \'%s\' failed: %s\n"
		, __func__, __LINE__, gpio_path, strerror(errno));
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

// 17 27 25 22
int gpio_read(const char *gpio_path, char *value)
{
	int fd;

	if ((fd = open(gpio_path, O_WRONLY)) < 0) {
		printf("[%s:%d] Opening \'%s\' failed: %s\n"
			, __func__, __LINE__, gpio_path, strerror(errno));
		return -1;
	}

	if (read(fd, value, 1) < 0) {
		printf("[%s:%d] Read from \'%s\' failed: %s\n"
		, __func__, __LINE__, gpio_path, strerror(errno));
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}
