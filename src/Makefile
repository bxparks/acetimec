CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic

OBJS := \
	ace_time_c/common.o \
	ace_time_c/epoch.o \
	ace_time_c/local_date.o \
	ace_time_c/local_date_time.o \
	ace_time_c/local_time.o \
	ace_time_c/offset_date_time.o \
	ace_time_c/string_buffer.o \
	ace_time_c/transition.o \
	ace_time_c/zone_info_utils.o \
	ace_time_c/zone_processing.o \
	ace_time_c/zone_registrar.o \
	ace_time_c/zoned_date_time.o \
	ace_time_c/zoned_extra.o \
	ace_time_c/zonedb/zone_infos.o \
	ace_time_c/zonedb/zone_policies.o \
	ace_time_c/zonedb/zone_registry.o

acetimec.a: $(OBJS)
	$(AR) -c -v -r $@ $(OBJS)

clean:
	rm -f $(OBJS) acetimec.a
