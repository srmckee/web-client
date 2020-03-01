#Makefile

PRE = g++ -Wall -g -std=c++11

ex15: ex15.o network.o
	$(PRE) -o $@ $^

ex15.o: ex15.cc
	$(PRE) -c $<

network.o: network.cc network.h
	$(PRE) -c $<

clean:
	rm -f *.o *~
