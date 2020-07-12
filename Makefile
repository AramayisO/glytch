CC = gcc
CFLAGS = -std=gnu99 -Wall

main: main.o mem_util.o child_proc.o rand_util.o classifier.o stats_util.o
	$(CC) $(CFLAGS) main.o mem_util.o child_proc.o rand_util.o classifier.o stats_util.o -o main -lm
	rm *.o

main.o: 
	$(CC) $(CFLAGS) -c src/main.c

mem_util.o: include/mem_util.h
	$(CC) $(CFLAGS) -c src/mem_util.c

child_proc.o: include/child_proc.h
	$(CC) $(CFLAGS) -c src/child_proc.c

rand_util.o: include/rand_util.h
	$(CC) $(CFLAGS) -c src/rand_util.c 

classifier.o: include/classifier.h
	$(CC) $(CFLAGS) -c src/classifier.c

stats_util.o: include/stats_util.h
	$(CC) $(CFLAGS) -c src/stats_util.c

run:
	./main

clean:
	rm *.o