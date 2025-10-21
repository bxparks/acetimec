CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic \
	-I../../acunit/src \
	-I../src

#-----------------------------------------------------------------------------
# Common sources and targets
#-----------------------------------------------------------------------------

SRCS := $(wildcard *.c)

OUTS := $(SRCS:.c=.out)

#-----------------------------------------------------------------------------
# Highres targets
#-----------------------------------------------------------------------------

# The Highres version is technically selected using
# 'ACE_TIME_C_ZONEDB_RES_HIGH', but it should be the default. Downstream
# clients are not expected to set this macro, so let's omit this from our tests
# to catch any potential bugs that would appear in downstream clients.
#HIGHRES_FLAGS := -D ACE_TIME_C_ZONEDB_RES=ACE_TIME_C_ZONEDB_RES_HIGH
HIGHRES_FLAGS :=
HIGHRES_DIR := highres
HIGHRES_OUTS := $(addprefix $(HIGHRES_DIR)/, $(OUTS))

#-----------------------------------------------------------------------------
# Midres targets
#-----------------------------------------------------------------------------

MIDRES_FLAGS := -D ACE_TIME_C_ZONEDB_RES=ACE_TIME_C_ZONEDB_RES_MID
MIDRES_DIR := midres
MIDRES_OUTS := $(addprefix $(MIDRES_DIR)/, $(OUTS))

#-----------------------------------------------------------------------------
# Run tests
#-----------------------------------------------------------------------------

all: $(HIGHRES_OUTS) $(MIDRES_OUTS)

runtests: runtestshigh runtestsmid

runtestshigh:
	set -e; \
	for i in $(HIGHRES_OUTS); do \
		echo '==== Running' $$i; \
		./$$i; \
	done

runtestsmid:
	set -e; \
	for i in $(MIDRES_OUTS); do \
		echo '==== Running' $$i; \
		./$$i; \
	done

#-----------------------------------------------------------------------------
# Highres dependents
#-----------------------------------------------------------------------------

HIGHRES_BUILD_PROXY := highres_build.touch

$(HIGHRES_DIR)/%.out: %.c ../src/acetimec.a $(HIGHRES_BUILD_PROXY)
	$(CC) $(CFLAGS) $(HIGHRES_FLAGS) -o $@ $^

# The touch file is a proxy for the res-specific build directory. If we try to
# depend directly on the directory, 'make' seems to build everything all the
# time.
$(HIGHRES_BUILD_PROXY):
	@touch $@
	@mkdir -p $(HIGHRES_DIR)

../src/acetimec.a:
	$(MAKE) -C ../src acetimec.a

#-----------------------------------------------------------------------------
# Midres dependents
#-----------------------------------------------------------------------------

MIDRES_BUILD_PROXY := midres_build.touch

$(MIDRES_DIR)/%.out: %.c ../src/acetimecm.a $(MIDRES_BUILD_PROXY)
	$(CC) $(CFLAGS) $(MIDRES_FLAGS) -o $@ $^

# The touch file is a proxy for the res-specific build directory. If we try to
# depend directly on the directory, 'make' seems to build everything all the
# time.
$(MIDRES_BUILD_PROXY):
	@touch $@
	@mkdir -p $(MIDRES_DIR)

../src/acetimecm.a:
	$(MAKE) -C ../src acetimecm.a

#-----------------------------------------------------------------------------

clean:
	rm -rf $(HIGHRES_DIR) $(HIGHRES_BUILD_PROXY) acetimec.a \
		$(MIDRES_DIR) $(MIDRES_BUILD_PROXY) acetimecm.a
