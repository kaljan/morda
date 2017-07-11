#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "periph.h"


int GPIO_Write(const char *gpio_path, const char *value)
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

int GPIO_Read(const char *gpio_path, char *value, size_t size)
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


int GPIO_Export(int pin)
{
	char s_pin[3];

	sprintf(s_pin, "%d", pin);

	if (GPIO_Write("/sys/class/gpio/export", s_pin) != 0) {
		printf("[%s:%d] Failed to export GPIO%d\n"
			, __func__, __LINE__, pin);
		return -1;
	}
	return 0;
}

int GPIO_Unexport(int pin)
{
	char s_pin[3];

	sprintf(s_pin, "%d", pin);

	if (GPIO_Write("/sys/class/gpio/unexport", s_pin) != 0) {
		printf("[%s:%d] Failed to unexport GPIO%d\n"
			, __func__, __LINE__, pin);
		return -1;
	}
	return 0;
}

int GPIO_SetDirection(int pin, const char *directoin)
{
	char s_pin_path[255];

	sprintf(s_pin_path, "/sys/class/gpio/gpio%d/direction", pin);

	if (GPIO_Write(s_pin_path, directoin) != 0) {
		printf("[%s:%d] Failed to set GPIO%d direction\n"
			, __func__, __LINE__, pin);
		return -1;
	}
	return 0;
}

int GPIO_SetValue(int pin, int value)
{
	char v[2];
	char s_pin_path[255];

	sprintf(s_pin_path, "/sys/class/gpio/gpio%d/value", pin);
	sprintf(v, "%d", value);

	if (GPIO_Write("/sys/class/gpio/gpio18/value", v) != 0) {
		printf("[%s:%d] Failed to set GPIO%d value\n"
			, __func__, __LINE__, pin);
		return -1;
	}
	return 0;
}

int GPIO_GetValue(int pin)
{
	char v[2] = "\0\0";
	char gpio_path[255];

	sprintf(gpio_path, "/sys/class/gpio/gpio%d/value", pin);

	if (GPIO_Read(gpio_path, v, 1) != 0) {
		printf("[%s:%d] Failed to get GPIO%d valuen"
			, __func__, __LINE__, pin);
		return -1;
	}
	return atoi(v);
}

GPIO_State GPIO_Debounce(int pin)
{
	int debounce_cnt = GPIO_DEBOUNCE_INIT;

	while (1) {
		if (GPIO_GetValue(pin) == 1) {
			debounce_cnt++;
		} else if (GPIO_GetValue(pin) == 0) {
			debounce_cnt--;
		} else {
			return GPIO_HIGH;
		}

		if (debounce_cnt > GPIO_DEBOUNCE_MAX) {
			return GPIO_HIGH;
		}

		if (debounce_cnt < GPIO_DEBOUNCE_MIN) {
			return GPIO_LOW;
		}
	}

}
