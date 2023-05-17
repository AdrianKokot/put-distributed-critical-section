SOURCES=$(wildcard *.c)
HEADERS=$(SOURCES:.c=.h)
# FLAGS=-DDEBUG -g
# FLAGS=-DDEBUG -g -DIGNORE_WAIT
FLAGS=-g
# FLAGS=-g -DIGNORE_WAIT -DQUEUE_PRINT
# FLAGS=-g -DIGNORE_WAIT

all: main tags

main: $(SOURCES) $(HEADERS) Makefile
	mpicc $(SOURCES) $(FLAGS) -o main

clear: clean

clean:
	rm main a.out

tags: ${SOURCES} ${HEADERS}
	ctags -R .

run: main Makefile tags
	mpirun -oversubscribe -np 4 ./main 3 2
	# mpirun -oversubscribe -np K ./main S N
