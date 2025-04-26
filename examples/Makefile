SHELL:=bash

# Ignore 'HelloTextClock' and 'MemoryBenchmark' because they depend on
# EpoxyDuino to compile Arduino binaries. But there are 2 copies of the
# acetimec and AceTime libraries, and EpoxyDuino currently cannot distinguish
# between them.
all:
	set -e; \
	for i in {hello_acetimec,multizone,validate_against_libc}/Makefile; do \
		echo '==== Making:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i); \
	done

clean:
	set -e; \
	for i in */Makefile; do \
		echo '==== Cleaning:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) clean; \
	done
