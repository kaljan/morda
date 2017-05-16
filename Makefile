
CC = gcc
CFLAGS = -Wall

INCLUDEPATH += \
	-I src \
	-I src/periph \
	-I src/ssd1311

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


gpio.o : src/periph/gpio.c src/periph/periph.h
	$(CC) $(CFLAGS) -c src/periph/gpio.c

i2c.o : src/periph/i2c.c src/periph/periph.h
	$(CC) $(CFLAGS) -c src/periph/i2c.c

ssd1311.o : src/ssd1311/ssd1311.c src/ssd1311/ssd1311.h src/periph/periph.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/ssd1311/ssd1311.c

si.o : src/si.c src/si.h
	$(CC) $(CFLAGS) -c src/si.c

scr.o : src/scr.c src/ssd1311/ssd1311.h src/si.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/scr.c

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c


clean:
	rm -rfv *.o

distclean:
	rm -rfv *.o $(TARGET)
