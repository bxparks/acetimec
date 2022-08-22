CFLAGS := -Wall -Wextra -Werror -ansi -pedantic -std=c11 -I../../ACUnit/src \
	-I../src

TARGETS := \
	common_test.out \
	local_date_test.out \
	local_date_time_test.out \
	offset_date_time_test.out \
	transition_test.out \
	zone_info_utils_test.out \
	zone_processing_test.out \
	zone_registrar_test.out \
	zoned_date_time_test.out \
	zoned_extra_test.out \
	zonedb_test.out

all: $(TARGETS)

#-----------------------------------------------------------------------------

common_test.out: common_test.o ../src/acetimec.a
	$(CC) -o $@ $^

local_date_test.out: local_date_test.o ../src/acetimec.a
	$(CC) -o $@ $^

local_date_time_test.out: local_date_time_test.o ../src/acetimec.a
	$(CC) -o $@ $^

offset_date_time_test.out: offset_date_time_test.o ../src/acetimec.a
	$(CC) -o $@ $^

transition_test.out: transition_test.o ../src/acetimec.a
	$(CC) -o $@ $^

zone_info_utils_test.out: zone_info_utils_test.o ../src/acetimec.a
	$(CC) -o $@ $^

zone_processing_test.out: zone_processing_test.o ../src/acetimec.a
	$(CC) -o $@ $^

zone_registrar_test.out: zone_registrar_test.o ../src/acetimec.a
	$(CC) -o $@ $^

zoned_date_time_test.out: zoned_date_time_test.o ../src/acetimec.a
	$(CC) -o $@ $^

zoned_extra_test.out: zoned_extra_test.o ../src/acetimec.a
	$(CC) -o $@ $^

zonedb_test.out: zonedb_test.o ../src/acetimec.a
	$(CC) -o $@ $^

#-----------------------------------------------------------------------------

common_test.o: common_test.c

local_date_test.o: local_date_test.c

local_date_time_test.o: local_date_time_test.c

offset_date_time_test.o: offset_date_time_test.c

transition_test.o: transition_test.c

zone_info_utils_test.o: zone_info_utils_test.c

zone_processing_test.o: zone_processing_test.c

zone_registrar_test.o: zone_registrar_test.c

zoned_date_time_test.o: zoned_date_time_test.c

zoned_extra_test.o: zoned_extra_test.c

zonedb_test.o: zonedb_test.c

#-----------------------------------------------------------------------------

runtests:
	set -e; \
	for i in $(TARGETS); do \
		echo '==== Running' $$i; \
		./$$i; \
	done

clean:
	rm -f *.o $(TARGETS)
