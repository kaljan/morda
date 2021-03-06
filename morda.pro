
CONFIG -= qt

INCLUDEPATH += \
	src \
	src/periph \
	src/ssd1311

SOURCES += \
	src/main.c \
	src/scr.c \
	src/si.c \
    src/ssd1311/ssd1311.c \
    src/periph/gpio.c \
    src/periph/i2c.c \
    src/buttons.c

HEADERS += \
    src/si.h \
    src/ssd1311/ssd1311.h \
    src/periph/periph.h \
    src/buttons.h

DISTFILES += \
	Makefile \
	.gitignore


