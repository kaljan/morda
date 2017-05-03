
CC = gcc
CFLAGS = -Wall

INCLUDEPATH += \
	-I src/ \
	-I src/hw/

DEFINES =

OBJECTS = \
	gpio.o \
	i2c.o \
	ssd1311.o \
	si.o \
	scr.o \
	main.o

TARGET = morda


$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)


gpio.o : src/hw/gpio.c
	$(CC) $(CFLAGS) -c src/hw/gpio.c

i2c.o : src/hw/i2c.c
	$(CC) $(CFLAGS) -c src/hw/i2c.c

ssd1311.o : src/hw/ssd1311.c src/hw/ssd1311.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/hw/ssd1311.c

si.o : src/si.c src/si.h
	$(CC) $(CFLAGS) -c src/si.c

scr.o : src/scr.c src/hw/ssd1311.h src/si.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/scr.c

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c


clean:
	rm -rfv *.o

distclean:
	rm -rfv *.o $(TARGET)
