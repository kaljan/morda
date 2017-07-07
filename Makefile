
CC = gcc
CFLAGS = -Wall -pthread

INCLUDEPATH += \
	-I src \
	-I src/periph \
	-I src/ssd1311

DEFINES =

OBJECTS = \
	gpio.o \
	i2c.o \
	ssd1311.o \
	buttons.o \
	si.o \
	btn.o \
	scr.o \
	main.o

TARGET = morda


$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDEPATH) -o $(TARGET) $(OBJECTS)

gpio.o : src/periph/gpio.c src/periph/periph.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/periph/gpio.c

i2c.o : src/periph/i2c.c src/periph/periph.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/periph/i2c.c

ssd1311.o : src/ssd1311/ssd1311.c src/ssd1311/ssd1311.h src/periph/periph.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/ssd1311/ssd1311.c

buttons.o : src/buttons.c src/buttons.h src/periph/periph.h
<<<<<<< HEAD
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/buttons.c
=======
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/buttons.c.
>>>>>>> 604a12688af1ec978e7719eb9be3628b29e43c50

si.o : src/si.c src/si.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/si.c

btn.o : src/btn.c src/periph/periph.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/btn.c

scr.o : src/scr.c src/ssd1311/ssd1311.h src/si.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/scr.c

main.o: src/main.c
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c src/main.c


clean:
	rm -rfv *.o

distclean:
	rm -rfv *.o $(TARGET)
