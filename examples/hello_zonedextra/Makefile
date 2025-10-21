CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic \
	-I../../src

hello_zonedextra.out: hello_zonedextra.c ../../src/acetimec.a
	$(CC) $(CFLAGS) -o $@ $^

run:
	./hello_zonedextra.out

#-----------------------------------------------------------------------------

../../src/acetimec.a:
	$(MAKE) -C ../../src acetimec.a

#-----------------------------------------------------------------------------

clean:
	rm -f *.o *.out
