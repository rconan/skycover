SRCS = point.cpp probe.cpp star.cpp stargroup.cpp polygon.cpp collisions.cpp prod.cpp

# O3 gave incorrect results on Linux
CFLAGS = -O
#CFLAGS = -g -gdwarf-2

CC   = g++

# CfA compiler location
#CC   = /opt/stow/gcc-6.2.0/bin/g++ -Wl,-rpath=/opt/stow/gcc-6.2.0/lib64 

all: probegeom skycov agwsvalid

probegeom:
	python agwsprobe.py 15

skycov: skycov.cpp collisions.cpp probe.cpp point.cpp stargroup.cpp prod.cpp star.cpp
	$(CC) $(CFLAGS) -std=c++11 $(SRCS) skycov.cpp -o skycov 

agwsvalid: agwsvalid.cpp collisions.cpp probe.cpp point.cpp stargroup.cpp prod.cpp star.cpp
	$(CC) $(CFLAGS) -std=c++11 $(SRCS) agwsvalid.cpp -o agwsvalid

clean:
	rm skycov agwsvalid
