CFLAGS := -std=c11 -Wall -Wextra -Werror \
	-I ../../src\
	-D _GNU_SOURCE

validate_against_libc.out: validate_against_libc.c ../../src/acetimec.a
	$(CC) $(CFLAGS) -o $@ $^

run:
	./validate_against_libc.out

#-----------------------------------------------------------------------------

../../src/acetimec.a:
	$(MAKE) -C ../../src acetimec.a

#-----------------------------------------------------------------------------

clean:
	rm -f *.o *.out
