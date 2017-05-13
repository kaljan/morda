#ifndef PERIPH_H
#define PERIPH_H


#include <stdint.h>


int gpio_write(const char *gpio_path, const char *value);
int gpio_read(const char *gpio_path, char *value);

int i2c_send_data(uint8_t addr, const uint8_t *buf, int size);

#endif // PERIPH_H
