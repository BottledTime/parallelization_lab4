CC=mpicc
CFLAGS=-g -Wall -lm
DEPS = Lab4_IO.c

main: PageRank.c
	$(CC) -o main PageRank.c $(DEPS) $(CFLAGS)
	$(CC) -o alternate PageRankDifferentPartition.c $(DEPS) $(CFLAGS)
