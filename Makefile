CC=g++
FLAGS=-std=c++17 -g -Wall -Wextra -Werror

all: main.cpp SmartCallback.hpp
	$(CC) $(FLAGS) -o $@ $^

clean:
	rm all *.o