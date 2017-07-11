#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "ssd1311.h"
#include "periph.h"


#define SSD1311_I2C_ADDR		0x3C
#define SSD1311_INIT_SEQ_SIZE	24


static const uint8_t SSD1311_InitSequence[SSD1311_INIT_SEQ_SIZE] = {
	0x80, SSD1311_CTRL,
	0x80, SSD1311_FNSET | SSD1311_FNSET_N | SSD1311_FNSET_RE,
	0x80, SSD1311_EX_CHZ | SSD1311_EX_CHZ_SD,
	0x80, SSD1311_EX_CLKCTRL,
	0x80, 0x70,
	0x80, SSD1311_EX_SETCC,
	0x80, 0xFF,
	0x80, SSD1311_EX_CHZ,
	0x80, SSD1311_EXFNSET,
	0x80, SSD1311_FNSET | SSD1311_FNSET_N,
	0x80, SSD1311_CLEAR,
	0x00, SSD1311_DDRAM_ADDR,
};

static const uint8_t SSD1311_OnSequence[2] = {0x00, 0x0C};
static uint8_t SSD1311_Packet[68];
static char SSD1311_String[33];


static int SSD1311_Reset(void)
{
	if (GPIO_Export(18) != 0) {
		return -1;
	}

	if (GPIO_SetDirection(18, "out") != 0) {
		return -1;
	}

	if (GPIO_SetValue(18, 1) != 0) {
		return -1;
	}

	usleep(50000);

	if (GPIO_SetValue(18, 0) != 0) {
		return -1;
	}

	usleep(50000);

	if (GPIO_SetValue(18, 1) != 0) {
		return -1;
	}

	return 0;
}

int SSD1311_Init(void)
{
	if (SSD1311_Reset() != 0) {
		printf("[%s:%d] Reset SSD1311 failed\n", __func__, __LINE__);
		return -1;
	}

	memset(SSD1311_String, ' ', 32);

	if (I2C_SendData(SSD1311_I2C_ADDR, SSD1311_InitSequence,
			SSD1311_INIT_SEQ_SIZE) != 0) {
		printf("[%s:%d] SSD1311 Initialization failed\n", __func__, __LINE__);
		return -1;
	}

	if (I2C_SendData(SSD1311_I2C_ADDR, SSD1311_OnSequence, 2) != 0) {
		printf("[%s:%d] Running SSD1311 failed\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

int SSD1311_Deinit(void)
{
	return GPIO_Unexport(18);
}

static void PrepareString(uint8_t *packet, const char *str)
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

int SSD1311_SetString(const char *str, int start)
{

	int i;
	char *sptr;

	i = start;
	sptr = &SSD1311_String[start];
	while (*str != 0) {
		*sptr = *str;
		sptr++;
		str++;
		i++;
		if (i >= 32) {
			break;
		}
	}
	PrepareString(SSD1311_Packet, SSD1311_String);
	return I2C_SendData(SSD1311_I2C_ADDR, SSD1311_Packet, 68);

}

int SSD1311_Clear(void)
{
	return 0;
}

int SSD1311_SetText(const char *text, int start, int size)
{
	memset(SSD1311_String, ' ', 32);
	strncpy(&SSD1311_String[start], text, size);

	PrepareString(SSD1311_Packet, SSD1311_String);
	return I2C_SendData(SSD1311_I2C_ADDR, SSD1311_Packet, 68);
}

int SSD1311_AppendText(const char *text, int start, int size)
{
	strncpy(&SSD1311_String[start], text, size);

	PrepareString(SSD1311_Packet, SSD1311_String);
	return I2C_SendData(SSD1311_I2C_ADDR, SSD1311_Packet, 68);
}

