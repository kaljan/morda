#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ssd1311.h"


extern int gpio_reset_ssd1311(void);
extern int gpio_reset_off(void);

extern int i2c_send_data(uint8_t addr, const uint8_t *buf, int size);


#define SSD1311_I2C_ADDR		0x3C
#define SSD1311_INIT_SEQ_SIZE	24


static const uint8_t ssd1311_init_seq[SSD1311_INIT_SEQ_SIZE] = {
	// Set Sleep Mode On
	0x80, SSD1311_CTRL,

	// **** Set "RE"= 1
	0x80, SSD1311_FNSET | SSD1311_FNSET_N | SSD1311_FNSET_RE,

	// Set "SD"=1
	0x80, SSD1311_EX_CHZ | SSD1311_EX_CHZ_SD,

	// Set Display Clock Divide Ratio/Oscillator Frequency
	0x80, SSD1311_EX_CLKCTRL,
	0x80, 0x70,

	// Set Contrast
	0x80, SSD1311_EX_SETCC,
	0x80, 0xFF,

	// Exiting Set OLED Characterization
	0x80, SSD1311_EX_CHZ,

	// Set Entry Mode
	0x80, SSD1311_EXFNSET,

	// Set "IS"=0 , "RE" = 0
	0x80, SSD1311_FNSET | SSD1311_FNSET_N,
	0x80, SSD1311_CLEAR,

	// Set DDRAM Address to 0x80 (line 1 start)
	0x00, SSD1311_DDRAM_ADDR,
};

static const uint8_t ssd1311_on_seq[2] = {0x00, 0x0C};
static uint8_t ssd1311_packet[68];
static char ssd1311_str[33];


static int ssd1311_on(void)
{
	return i2c_send_data(SSD1311_I2C_ADDR, ssd1311_on_seq, 2);
}

int ssd1311_init(void)
{

	if (gpio_reset_ssd1311() != 0) {
		printf("[%s:%d] Reset SSD1311 failed\n", __func__, __LINE__);
		return -1;
	}

	memset(ssd1311_str, ' ', 32);

	if (i2c_send_data(SSD1311_I2C_ADDR, ssd1311_init_seq,
			SSD1311_INIT_SEQ_SIZE) != 0) {
		return -1;
	}

	if (ssd1311_on() != 0) {
		printf("[%s:%d] Running SSD1311 failed\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

int ssd1311_deinit(void)
{
	if (gpio_reset_off() != 0) {
		return -1;
	}
	return 0;
}

static void prepare_str(uint8_t *packet, const char *str)
{
	int i;
	*packet = 0x80;
	packet++;
	*packet = 0x80;

	for (i = 0; i < 16; i++) {
		packet++;
		*packet = 0xC0;
		packet++;
		*packet = *str;
		str++;
	}

	packet++;
	*packet = 0x80;
	packet++;
	*packet = 0xC0;

	for (i = 0; i < 16; i++) {
		packet++;
		*packet = ((i < 15) ? 0xC0 : 0x40);
		packet++;
		*packet = *str;
		str++;
	}
}

int ssd1311_set_string(const char *str, int start)
{

	int i;
	char *sptr;

	i = start;
	sptr = &ssd1311_str[start];
	while (*str != 0) {
		*sptr = *str;
		sptr++;
		str++;
		i++;
		if (i >= 32) {
			break;
		}
	}
	prepare_str(ssd1311_packet, ssd1311_str);
	return i2c_send_data(SSD1311_I2C_ADDR, ssd1311_packet, 68);

}
