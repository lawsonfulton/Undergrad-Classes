TARGET = prog
INCLUDES = -I/usr/local/Cellar/graphviz/2.38.0/include/graphviz/
LIBS = -lm -L/usr/local/Cellar/graphviz/2.38.0/lib/
CC = g++
CFLAGS = -g -Wall -O3

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)