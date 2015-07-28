CC=gcc
CFLAGS=-c -Wall -I/usr/include
LDFLAGS= -L/usr/lib
LIBS= -lgsl -lgslcblas -lm
SOURCES=example-ode.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=example-ode

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
