CFLAGS := -Wall -Wextra -Werror -ansi -pedantic -std=c11

OBJS := ace_time_c/local_date.o ace_time_c/local_date_time.o

acetimec.a: $(OBJS)
	$(AR) -c -v -r $@ $(OBJS)

clean:
	rm -f $(OBJS) acetimec.a
