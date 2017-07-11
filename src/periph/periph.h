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


int GPIO_Write(const char *gpio_path, const char *value);
int GPIO_Read(const char *gpio_path, char *value, size_t size);

int GPIO_Export(int pin);
int GPIO_Unexport(int pin);
int GPIO_SetDirection(int pin, const char *directoin);
int GPIO_SetValue(int pin, int value);
int GPIO_GetValue(int pin);

int I2C_SendData(uint8_t addr, const uint8_t *buf, int size);
GPIO_State GPIO_Debounce(int pin);

#endif // PERIPH_H
