CFLAGS := -D _GNU_SOURCE -Wall -Wextra -std=c11

multizone.out: multizone.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o *.out
