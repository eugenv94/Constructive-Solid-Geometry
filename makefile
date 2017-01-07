CC := g++ -std=c++11
SRCS := $(wildcard *.hpp) main.cpp
INCLUDES := -I/usr/local/include -L/usr/local/lib
FLAGS = -lglew -framework OpenGL -framework GLUT -framework Cocoa -w

build:
	$(CC) $(SRCS) $(INCLUDES) $(FLAGS)

run:
	./a.out

build-and-run:
	make build
	make run

clean:
	rm a.out *gch
