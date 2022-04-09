CFLAGS := -Wall -Wextra -Werror -ansi -pedantic -std=c11

OBJS := ace_time_c/date_utils.o ace_time_c/epoch_converter_hinnant2.o

acetimec.a: $(OBJS)
	$(AR) -c -v -r $@ $(OBJS)

clean:
	rm -f $(OBJS) acetimec.a
