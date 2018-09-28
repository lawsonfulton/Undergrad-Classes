TARGET = sploit1
INCLUDES = 
LIBS = -lm 
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)


#gcc -Wall -ggdb sploitX.c -o sploitX 
#gcc -Wall -ggdb sploit1.c -o sploit1 
#will build your sploit programs on the uml virtual machine

# Execution will be from a clean /share directory on the virtual
# environment as fol- lows: ./sploitX (where X=1..4)
