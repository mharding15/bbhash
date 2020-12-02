#Makefile for “bbhash” C++ CMSC858D - HW2
#Created by Matt Harding 11/27/20

PROG = bbhash
CC = g++
CPPFLAGS = -g --std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib
OBJS = main.o bbhash.o
LDFLAGS = -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LDFLAGS)
main.o :
	$(CC) $(CPPFLAGS) -c main.cpp
bbhash.o : bbhash.h
	$(CC) $(CPPFLAGS) -c bbhash.cpp
clean:
	rm -f core $(PROG) $(OBJS)
