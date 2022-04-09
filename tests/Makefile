CFLAGS := -Wall -Wextra -Werror -ansi -pedantic -std=c11 -I ../src

TARGETS := epoch_converters_test.out

epoch_converters_test.out: epoch_converters_test.o ../src/acetimec.a
	$(CC) -o $@ $^

epoch_converters_test.o: epoch_converters_test.c minunit.h

runtests:
	./epoch_converters_test.out

clean:
	rm -f *.o $(TARGETS)
