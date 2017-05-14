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

int gpio_read(const char *gpio_path, char *value, size_t size)
{
	int fd;

	if ((fd = open(gpio_path, O_RDONLY)) < 0) {
		printf("[%s:%d] Opening \'%s\' failed: %s\n"
			, __func__, __LINE__, gpio_path, strerror(errno));
		return -1;
	}

	if (read(fd, value, size) < 0) {
		printf("[%s:%d] Read from \'%s\' failed: %s\n"
		, __func__, __LINE__, gpio_path, strerror(errno));
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}


int gpio_export(int pin)
{
	char s_pin[3];

	sprintf(s_pin, "%d", pin);

	if (gpio_write("/sys/class/gpio/export", s_pin) != 0) {
		printf("[%s:%d] Failed to export GPIO%d\n"
			, __func__, __LINE__, pin);
		return -1;
	}
	return 0;
}

int gpio_unexport(int pin)
{
	char s_pin[3];

	sprintf(s_pin, "%d", pin);

	if (gpio_write("/sys/class/gpio/unexport", s_pin) != 0) {
		printf("[%s:%d] Failed to unexport GPIO%d\n"
			, __func__, __LINE__, pin);
		return -1;
	}
	return 0;
}

int gpio_set_direction(int pin, const char *directoin)
{
	char s_pin_path[255];

	sprintf(s_pin_path, "/sys/class/gpio/gpio%d/direction", pin);

	if (gpio_write(s_pin_path, directoin) != 0) {
		printf("[%s:%d] Failed to set GPIO%d direction\n"
			, __func__, __LINE__, pin);
		return -1;
	}
	return 0;
}

int gpio_set_value(int pin, int value)
{
	char v[2];
	char s_pin_path[255];

	sprintf(s_pin_path, "/sys/class/gpio/gpio%d/value", pin);
	sprintf(v, "%d", value);

	if (gpio_write("/sys/class/gpio/gpio18/value", v) != 0) {
		printf("[%s:%d] Failed to set GPIO%d value\n"
			, __func__, __LINE__, pin);
		return -1;
	}
	return 0;
}

int gpio_get_value(int pin)
{
	char v[2] = "\0\0";
	char gpio_path[255];

	sprintf(gpio_path, "/sys/class/gpio/gpio%d/value", pin);

	if (gpio_read(gpio_path, v, 1) != 0) {
		printf("[%s:%d] Failed to get GPIO%d valuen"
			, __func__, __LINE__, pin);
		return -1;
	}
	return atoi(v);
}

