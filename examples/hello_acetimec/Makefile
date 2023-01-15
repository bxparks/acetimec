CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic \
	-I../../src

TARGETS := hello_acetimec.out

all: $(TARGETS)

#-----------------------------------------------------------------------------
# Copy the acetimec.a library here. The sole purpose of this rule is to force
# the acetime.a file to be regenerated if necessary. The .PHONY rule causes it
# to always run. This indirection rule of copying the *.a file here prevents
# all the *.o files from recompiled if the acetimec.a file was not changed.

.PHONY: ../../src/acetimec.a

acetimec.a: ../../src/acetimec.a
	$(MAKE) -C ../../src
	cp -a $< $@

#-----------------------------------------------------------------------------

hello_acetimec.out: hello_acetimec.o acetimec.a
	$(CC) -o $@ $^

#-----------------------------------------------------------------------------

hello_acetimec.o: hello_acetimec.c acetimec.a

#-----------------------------------------------------------------------------

run:
	./hello_acetimec.out

clean:
	rm -f *.o $(TARGETS)
