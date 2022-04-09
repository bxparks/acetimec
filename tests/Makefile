CFLAGS := -Wall -Wextra -Werror -ansi -pedantic -std=c11 -I ../src

TARGETS := local_date_test.out

local_date_test.out: local_date_test.o ../src/acetimec.a
	$(CC) -o $@ $^

local_date.o: local_date_test.c minunit.h

runtests:
	./local_date_test.out

clean:
	rm -f *.o $(TARGETS)
