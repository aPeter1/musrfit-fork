# Makefile for nexus_read_test.cpp
# $Id$

NEXUS_CLASS     = PNeXus
NEXUS_CLASS_DIR = ../../external/nexus

CXX      = g++
CXXFLAGS = -g -Wall -fPIC
INCLUDES = -I /opt/nexus/include -I $(NEXUS_CLASS_DIR)
LD       = g++
LDFLAGS  = -g

LIBS     = -L /opt/nexus/lib -lNeXus
LIBS    += -L /opt/hdf/lib -lhdf5

EXEC = nexus_read_test

all: $(EXEC)

$(EXEC): $(NEXUS_CLASS).o $(EXEC).o 
	$(LD) $(LDFLAGS) $(NEXUS_CLASS).o $(EXEC).o -o $(EXEC) $(LIBS)
	
$(EXEC).o: $(EXEC).cpp
	$(CXX) -c $(INCLUDES) $(CXXFLAGS) $(EXEC).cpp
	
$(NEXUS_CLASS).o: $(NEXUS_CLASS_DIR)/$(NEXUS_CLASS).cpp	
	$(CXX) -c $(INCLUDES) $(CXXFLAGS) $(NEXUS_CLASS_DIR)/$(NEXUS_CLASS).cpp
	
clean:
	@rm -f *.o


