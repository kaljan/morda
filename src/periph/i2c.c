#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "periph.h"

const char I2C_FileName[]="/dev/i2c-1";
int I2C_Fd;

int I2C_SendData(uint8_t addr, const uint8_t *buf, int size)
{
	if ((I2C_Fd = open(I2C_FileName, O_RDWR)) < 0) {
		printf("Failed to open %s: %s\n", I2C_FileName, strerror(errno));
		return -1;
	}

	if (ioctl(I2C_Fd, I2C_SLAVE, addr) < 0) {
		close(I2C_Fd);
		printf("I2C Connection failed: %s\n", strerror(errno));
		return -1;
	}

	if (write(I2C_Fd, buf, size) < 0) {
		close(I2C_Fd);
		printf("[%s, %d] I2C write failed: %s\n"
		, __func__, __LINE__, strerror(errno));;
		return -1;
	}

	close(I2C_Fd);

	return 0;
}
