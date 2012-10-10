CC = g++
LPATH = -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
CFLAGS=-g -Wall -Wextra -pedantic
LIBS=-lGL -lglut -lGLU -ljpeg -lpng15 -lm
IPATH= -I/usr/pkg/include

all: Assign5
Assign5 : G308_ImageLoader.o main.o G308_Geometry.o quaternion.o #TextureDemo.o 
	$(CC) -o $@ $^ $(LIBS) $(LPATH) $(LDPATH)
.cpp.o: 
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)
clean :
	rm -rf *.o
	rm Assign5
