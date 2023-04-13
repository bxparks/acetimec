CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic

.PHONY: zonedbs

OBJS := \
	zoneinfo/zone_info_utils.o \
	acetimec/common.o \
	acetimec/date_tuple.o \
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
	zonedb/zone_registry.o \
	zonedball/zone_infos.o \
	zonedball/zone_policies.o \
	zonedball/zone_registry.o \
	zonedbtesting/zone_infos.o \
	zonedbtesting/zone_policies.o \
	zonedbtesting/zone_registry.o

acetimec.a: $(OBJS) Makefile
	$(AR) -c -v -r $@ $(OBJS)

zonedbs:
	make -C zonedb
	make -C zonedball
	make -C zonedbtesting

clean:
	rm -f $(OBJS) acetimec.a
