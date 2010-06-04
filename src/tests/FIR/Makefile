# Makefile for the remez exchange algorithm
#
# Author: Andreas Suter
# e-mail: andreas.suter@psi.ch
#
# $Id$
#----------------------------------------------------------------

CXX      = gcc
CXXFLAGS = -g -Wall -fPIC
LD       = g++
LDFLAGS  = -g

LIBS     = -lm

OBJS     = remez.o
OBJS    += test.o

EXEC     = remez_test

all: $(EXEC)

$(EXEC): $(OBJS)
	$(LD) $(OBJS) -o $(EXEC) $(LIBS)
	
clean:
	@rm -f $(OBJS)
	@echo ">> removing $(OBJS)"
	
$(OBJS): %.o: %.c
	$(CXX) $(CXXFLAGS) -c $<	
