CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic \
	-D ATC_HIRES_ZONEDB=1 \
	-I../../ACUnit/src \
	-I../src

TARGETS := \
	common_test.out \
	date_tuple_test.out \
	epoch_test.out \
	local_date_test.out \
	local_date_time_test.out \
	local_time_test.out \
	offset_date_time_test.out \
	string_buffer_test.out \
	time_zone_test.out \
	transition_test.out \
	zone_info_utils_test.out \
	zone_processor_test.out \
	zone_processor_transition_test.out \
	zone_registrar_test.out \
	zoned_date_time_test.out \
	zoned_extra_test.out \
	zonedb_test.out

all: $(TARGETS)

#-----------------------------------------------------------------------------
# Copy the acetimec.a library here. The sole purpose of this rule is to force
# the acetime.a file to be regenerated if necessary. The .PHONY rule causes it
# to always run. This indirection rule of copying the *.a file here prevents
# all the *.o files from recompiled if the acetimec.a file was not changed.

.PHONY: ../src/acetimec.a

acetimec.a: ../src/acetimec.a
	$(MAKE) -C ../src
	cp -a $< $@

#-----------------------------------------------------------------------------

common_test.out: common_test.o acetimec.a
	$(CC) -o $@ $^

date_tuple_test.out: date_tuple_test.o acetimec.a
	$(CC) -o $@ $^

epoch_test.out: epoch_test.o acetimec.a
	$(CC) -o $@ $^

local_date_test.out: local_date_test.o acetimec.a
	$(CC) -o $@ $^

local_date_time_test.out: local_date_time_test.o acetimec.a
	$(CC) -o $@ $^

local_time_test.out: local_time_test.o acetimec.a
	$(CC) -o $@ $^

offset_date_time_test.out: offset_date_time_test.o acetimec.a
	$(CC) -o $@ $^

string_buffer_test.out: string_buffer_test.o acetimec.a
	$(CC) -o $@ $^

time_zone_test.out: time_zone_test.o acetimec.a
	$(CC) -o $@ $^

transition_test.out: transition_test.o acetimec.a
	$(CC) -o $@ $^

zone_info_utils_test.out: zone_info_utils_test.o acetimec.a
	$(CC) -o $@ $^

zone_processor_test.out: zone_processor_test.o acetimec.a
	$(CC) -o $@ $^

zone_processor_transition_test.out: zone_processor_transition_test.o acetimec.a
	$(CC) -o $@ $^

zone_registrar_test.out: zone_registrar_test.o acetimec.a
	$(CC) -o $@ $^

zoned_date_time_test.out: zoned_date_time_test.o acetimec.a
	$(CC) -o $@ $^

zoned_extra_test.out: zoned_extra_test.o acetimec.a
	$(CC) -o $@ $^

zonedb_test.out: zonedb_test.o acetimec.a
	$(CC) -o $@ $^

#-----------------------------------------------------------------------------

common_test.o: common_test.c acetimec.a

date_tuple_test.o: date_tuple_test.c acetimec.a

epoch_test.o: epoch_test.c acetimec.a

local_date_test.o: local_date_test.c acetimec.a

local_date_time_test.o: local_date_time_test.c acetimec.a

offset_date_time_test.o: offset_date_time_test.c acetimec.a

string_buffer.o: string_buffer.c acetimec.a

time_zone_test.o: time_zone_test.c acetimec.a

transition_test.o: transition_test.c acetimec.a

zone_info_utils_test.o: zone_info_utils_test.c acetimec.a

zone_processor_test.o: zone_processor_test.c acetimec.a

zone_processor_transition_test.o: zone_processor_transition_test.c acetimec.a

zone_registrar_test.o: zone_registrar_test.c acetimec.a

zoned_date_time_test.o: zoned_date_time_test.c acetimec.a

zoned_extra_test.o: zoned_extra_test.c acetimec.a

zonedb_test.o: zonedb_test.c acetimec.a

#-----------------------------------------------------------------------------

runtests:
	set -e; \
	for i in $(TARGETS); do \
		echo '==== Running' $$i; \
		./$$i; \
	done

clean:
	rm -f *.o $(TARGETS) acetimec.a
