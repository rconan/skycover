SRCS = point.cpp probe.cpp star.cpp stargroup.cpp polygon.cpp collisions.cpp prod.cpp
CC   = g++

skycov: skycov.cpp
	$(CC) -std=c++11 $(SRCS) skycov.cpp -o skycov 
