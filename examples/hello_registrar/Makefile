CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic \
	-I../../src

hello_registrar.out: hello_registrar.c ../../src/acetimec.a
	$(CC) $(CFLAGS) -o $@ $^

run:
	./hello_registrar.out

#-----------------------------------------------------------------------------

../../src/acetimec.a:
	$(MAKE) -C ../../src acetimec.a

#-----------------------------------------------------------------------------

clean:
	rm -f *.o *.out
