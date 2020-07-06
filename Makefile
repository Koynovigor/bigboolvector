all: clean test build

build: bigbool.c bool.o
	@gcc -g -Wall -o bigbool ./bigbool.c ./bool.o

bool.o: bool.c bool.h
	@gcc -g -c -o bool.o ./bool.c

test: test_criterion

test_criterion: criterion.c bool.o
	@gcc -lcriterion -o test_criterion ./criterion.c ./bool.o
	@./test_criterion

test_de_Morgan: test_operation

test_operation: de_Morgan.c bool.o
	@gcc -lcriterion -o test_de_Morgan ./de_Morgan.c ./bool.o
	@./test_de_Morgan

clean:
	@rm -f ./bool.o

