
CONFIG -= qt

INCLUDEPATH += \
	src \
	src/hw \

SOURCES += \
	src/hw/gpio.c \
	src/hw/i2c.c \
	src/hw/ssd1311.c \
	src/main.c \
	src/scr.c \
	src/si.c

HEADERS += \
	src/hw/ssd1311.h \
    src/si.h

DISTFILES += \
	Makefile \
	.gitignore


