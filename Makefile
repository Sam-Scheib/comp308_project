CC = g++
LPATH = -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
CFLAGS=-g -Wall -Wextra -pedantic
LIBS=-lGL -lglut -lGLU -ljpeg -lm -lGLEW -lpng15
IPATH= -I/usr/pkg/include

all: Project
Project : main.o quaternion.o fluid.o shader.o FBO.o imageLoader.o ball.o G308_Skeleton.o OctTree.o
	$(CC) -o $@ $^ $(LIBS) $(LPATH) $(LDPATH)
.cpp.o: 
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)
clean :
	rm -rf *.o
	rm -f Project
