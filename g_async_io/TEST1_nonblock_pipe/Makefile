CC=gcc
CFLAGS= -g `pkg-config --cflags glib-2.0`
LDFLAGS= -g `pkg-config --libs glib-2.0`
SOURCES=src.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test.bin

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -pthread -o $@ $(LDFLAGS)

.o:
	$(CC) $< -pthread -o $@ $(CFLAGS)

clean:
	rm $(OBJECTS) $(EXECUTABLE)
