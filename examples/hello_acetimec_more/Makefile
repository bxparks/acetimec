CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic \
	-I../../src

hello_acetimec_more.out: hello_acetimec_more.c ../../src/acetimec.a
	$(CC) $(CFLAGS) -o $@ $^

run:
	./hello_acetimec_more.out

#-----------------------------------------------------------------------------

../../src/acetimec.a:
	$(MAKE) -C ../../src acetimec.a

#-----------------------------------------------------------------------------

clean:
	rm -f *.o *.out
