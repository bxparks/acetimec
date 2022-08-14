CFLAGS := -Wall -Wextra -Werror -ansi -pedantic -std=c11 -I ../src

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

zonedb_test.out: zonedb_test.o ../src/acetimec.a
	$(CC) -o $@ $^

#-----------------------------------------------------------------------------

common_test.o: common_test.c acunit.h

local_date_test.o: local_date_test.c acunit.h

local_date_time_test.o: local_date_time_test.c acunit.h

offset_date_time_test.o: offset_date_time_test.c acunit.h

transition_test.o: transition_test.c acunit.h

zone_info_utils_test.o: zone_info_utils_test.c acunit.h

zone_processing_test.o: zone_processing_test.c acunit.h

zone_registrar_test.o: zone_registrar_test.c acunit.h

zoned_date_time_test.o: zoned_date_time_test.c acunit.h

zonedb_test.o: zonedb_test.c acunit.h

#-----------------------------------------------------------------------------

runtests:
	set -e; \
	for i in $(TARGETS); do \
		echo '==== Running' $$i; \
		./$$i; \
	done

clean:
	rm -f *.o $(TARGETS)
