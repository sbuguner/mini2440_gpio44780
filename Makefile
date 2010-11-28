CC = arm-none-linux-gnueabi-gcc
CFLAGS = -Wall -march=armv4t
LFLAGS = -march=armv4t
OBJS = main.o hd44780.o
TARGET = test

$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) -o $(TARGET) $(OBJS)  

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $< 
	
clean:
	-rm *.o 