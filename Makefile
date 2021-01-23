be:be.o
	g++ be.o -O3 -o be
be.o:be.cpp core_type.h trans_in_mem.h
	g++ -c be.cpp -O3
