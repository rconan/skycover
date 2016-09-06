SRCS = point.cpp probe.cpp star.cpp stargroup.cpp polygon.cpp collisions.cpp prod.cpp
CC   = g++

skycov: skycov.cpp collisions.cpp probe.cpp point.cpp stargroup.cpp prod.cpp star.cpp
	$(CC) -std=c++11 $(SRCS) skycov.cpp -o skycov 
