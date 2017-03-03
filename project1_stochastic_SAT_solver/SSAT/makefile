CXX = g++
CXXFLAGS = -Wall

all: ssat.cpp ssat.h clauses.cpp clauses.h rtimer.h
	$(CXX) $(CXXFLAGS) -O $! ssat.cpp clauses.cpp rtimer.c

clean:
	rm -f ssat
