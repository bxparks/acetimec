CFLAGS := -Wall -Wextra -Werror -ansi -pedantic -std=c11

OBJS := \
	ace_time_c/common.o \
	ace_time_c/local_date.o \
	ace_time_c/local_date_time.o \
	ace_time_c/zone_processing.o \
	ace_time_c/zoned_date_time.o \
	ace_time_c/zonedb/zone_infos.o \
	ace_time_c/zonedb/zone_policies.o \
	ace_time_c/zonedb/zone_registry.o

acetimec.a: $(OBJS)
	$(AR) -c -v -r $@ $(OBJS)

clean:
	rm -f $(OBJS) acetimec.a
