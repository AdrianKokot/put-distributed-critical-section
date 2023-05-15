SOURCES=$(wildcard *.c)
HEADERS=$(SOURCES:.c=.h)
# FLAGS=-DDEBUG -g
# FLAGS=-DDEBUG -g -DIGNORE_WAIT
# FLAGS=-g
FLAGS=-g -DIGNORE_WAIT

all: main tags

main: $(SOURCES) $(HEADERS) Makefile
	mpicc $(SOURCES) $(FLAGS) -o main

clear: clean

clean:
	rm main a.out

tags: ${SOURCES} ${HEADERS}
	# ctags -R .

run: main Makefile tags
	mpirun -oversubscribe -np 8 ./main 4 2
	# mpirun -oversubscribe -np K ./main S N
