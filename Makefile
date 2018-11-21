# Kyle Schultz
CXX = gcc

CXXFLAGS =  -Wall -Wextra -pthread

all: multi-lookup

multi-lookup:
	$(CXX) $(CXXFLAGS) -o multi-lookup multi-lookup.c

clean:
	rm multi-lookup

