#ifndef PERIPH_H
#define PERIPH_H

#include <string.h>
#include <stdint.h>


#define GPIO_DEBOUNCE_INIT	1024
#define GPIO_DEBOUNCE_MAX	1536
#define GPIO_DEBOUNCE_MIN	512


typedef enum _GPIO_State {
	GPIO_LOW = 0,
	GPIO_HIGH
} GPIO_State;


int gpio_write(const char *gpio_path, const char *value);
int gpio_read(const char *gpio_path, char *value, size_t size);

int gpio_export(int pin);
int gpio_unexport(int pin);
int gpio_set_direction(int pin, const char *directoin);
int gpio_set_value(int pin, int value);
int gpio_get_value(int pin);

int i2c_send_data(uint8_t addr, const uint8_t *buf, int size);
GPIO_State GPIO_Debounce(int pin);

#endif // PERIPH_H
