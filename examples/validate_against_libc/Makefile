CFLAGS := -std=c11 -Wall -Wextra -Werror \
	-I ../../src\
	-D _GNU_SOURCE

TARGETS := hello_acetimec.out

.PHONY: ../../src/acetimec.a

validate_against_libc.out: validate_against_libc.o ../../src/acetimec.a
	$(CC) -o $@ $^

validate_against_libc.o: validate_against_libc.c acetimec.a

# Copy the acetimec.a library here. The sole purpose of this rule is to force
# the acetime.a file to be regenerated if necessary. The .PHONY rule causes it
# to always run. This indirection rule of copying the *.a file here prevents
# all the *.o files from recompiled if the acetimec.a file was not changed.
acetimec.a: ../../src/acetimec.a
	$(MAKE) -C ../../src
	cp -a $< $@

clean:
	rm -f *.o *.out *.a
