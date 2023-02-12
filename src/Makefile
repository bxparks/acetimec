CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic

OBJS := \
	zoneinfo/zone_info_utils.o \
	acetimec/common.o \
	acetimec/epoch.o \
	acetimec/local_date.o \
	acetimec/local_date_time.o \
	acetimec/local_time.o \
	acetimec/offset_date_time.o \
	acetimec/string_buffer.o \
	acetimec/time_zone.o \
	acetimec/transition.o \
	acetimec/zone_processor.o \
	acetimec/zone_registrar.o \
	acetimec/zoned_date_time.o \
	acetimec/zoned_extra.o \
	zonedb/zone_infos.o \
	zonedb/zone_policies.o \
	zonedb/zone_registry.o

acetimec.a: $(OBJS)
	$(AR) -c -v -r $@ $(OBJS)

clean:
	rm -f $(OBJS) acetimec.a
