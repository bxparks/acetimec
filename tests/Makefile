CFLAGS := -Wall -Wextra -Werror -ansi -pedantic -std=c11 -I ../src

TARGETS := local_date_test.out local_date_time_test.out

all: $(TARGETS)

local_date_test.out: local_date_test.o ../src/acetimec.a
	$(CC) -o $@ $^

local_date_time_test.out: local_date_time_test.o ../src/acetimec.a
	$(CC) -o $@ $^

local_date_test.o: local_date_test.c acunit.h

local_date_time_test.o: local_date_time_test.c acunit.h

runtests:
	set -e; \
	for i in $(TARGETS); do \
		echo '==== Running' $$i; \
		./$$i; \
	done

clean:
	rm -f *.o $(TARGETS)
