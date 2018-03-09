CC=mpicc
CFLAGS=-g -Wall -lm
DEPS = Lab4_IO.c

main: PageRank.c
	$(CC) -o pagerank PageRank.c $(DEPS) $(CFLAGS)
