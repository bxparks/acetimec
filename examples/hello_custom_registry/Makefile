CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic \
	-I../../src

hello_custom_registry.out: hello_custom_registry.c ../../src/acetimec.a
	$(CC) $(CFLAGS) -o $@ $^

run:
	./hello_custom_registry.out

#-----------------------------------------------------------------------------

../../src/acetimec.a:
	$(MAKE) -C ../../src acetimec.a

#-----------------------------------------------------------------------------

clean:
	rm -f *.o *.out
