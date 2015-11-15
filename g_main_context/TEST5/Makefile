CC=gcc
CFLAGS= -g $(shell pkg-config --cflags glib-2.0 gtk+-2.0)
LDFLAGS= -g $(shell pkg-config --libs glib-2.0 gtk+-2.0)
SOURCES=source5.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test5

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -pthread -o $@ -I/usr/include/glib-2.0 -lglib-2.0

.o:
	$(CC) $(CFLAGS) $< -pthread -o $@ -I/usr/include/glib-2.0 -lglib-2.0

clean:
	rm $(OBJECTS) $(EXECUTABLE)
