CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic \
	-I../../src

hello_acetimec.out: hello_acetimec.c ../../src/acetimec.a
	$(CC) $(CFLAGS) -o $@ $^

run:
	./hello_acetimec.out

#-----------------------------------------------------------------------------

../../src/acetimec.a:
	$(MAKE) -C ../../src acetimec.a

#-----------------------------------------------------------------------------

clean:
	rm -f *.o *.out
