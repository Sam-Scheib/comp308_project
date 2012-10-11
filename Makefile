CC = g++
LPATH = -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
CFLAGS=-g -Wall -Wextra -pedantic
LIBS=-lGL -lglut -lGLU -ljpeg -lpng15 -lm
IPATH= -I/usr/pkg/include

all: Project
Project : main.o quaternion.o fluid.o imageLoader.o
	$(CC) -o $@ $^ $(LIBS) $(LPATH) $(LDPATH)
.cpp.o: 
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)
clean :
	rm -rf *.o
	rm Project
