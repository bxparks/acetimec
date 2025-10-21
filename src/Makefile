#------------------------------------------------------------------------------
# Rules to generate both HIGHRES and MIDRES versions of acetimec.a using
# two different build directories to hold the intermediate *.o files.
#
# HIGHRES (default):
#	- archive: acetimec.a
#	- build directory: ./highres/
# MIDRES:
#	- achive files: acetimecm.a
#	- build directory: ./midres/
#
# Inspired by https://stackoverflow.com/questions/1079832.
#------------------------------------------------------------------------------

CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic

#------------------------------------------------------------------------------
# List of source files
#------------------------------------------------------------------------------

# Project sources

COMMON_SRCS := $(wildcard \
	acetimec/*.c \
	zoneinfo/*.c \
)

HIGHRES_SRCS := $(COMMON_SRCS) $(wildcard \
	zonedball/*.c \
	zonedb2000/*.c \
	zonedb2025/*.c \
	zonedbtesting/*.c \
)

MIDRES_SRCS := $(COMMON_SRCS) $(wildcard \
	zonedb2000m/*.c \
	zonedb2025m/*.c \
	zonedbtestingm/*.c \
)

# Project header files

HIGHRES_HEADERS := $(HIGH_SRCS:.c=.h) \
	zoneinfo/zone_info.h \
	zoneinfo/zone_info_mid.h \
	zoneinfo/zone_info_high.h

MIDRES_HEADERS := $(MID_SRCS:.c=.h) \
	zoneinfo/zone_info.h \
	zoneinfo/zone_info_mid.h \
	zoneinfo/zone_info_high.h

#------------------------------------------------------------------------------

all: acetimec.a acetimecm.a

#------------------------------------------------------------------------------
# Highres files
#------------------------------------------------------------------------------

# Highres settings. Highres is technically selected using the '-D
# ACE_TIME_C_ZONEDB_RES=ACE_TIME_C_ZONEDB_RES_HIGH', but it is also the
# default. Downstream clients are *not* expected to use this macro, so we
# omitted it here as well, to catch any bugs that would appear in downstream
# clients.
HIGHRES_DIR := highres
HIGHRES_DIR_PROXY := highres_dir.touch
HIGHRES_BARE_OBJS := $(HIGHRES_SRCS:.c=.o)
HIGHRES_OBJS := $(addprefix $(HIGHRES_DIR)/, $(HIGHRES_BARE_OBJS))
#HIGHRES_FLAGS := -D ACE_TIME_C_ZONEDB_RES=ACE_TIME_C_ZONEDB_RES_HIGH
HIGHRES_FLAGS :=
HIGHRES_BUILD_DIRS := \
	$(HIGHRES_DIR)/acetimec \
	$(HIGHRES_DIR)/zoneinfo \
	$(HIGHRES_DIR)/zonedball \
	$(HIGHRES_DIR)/zonedb2000 \
	$(HIGHRES_DIR)/zonedb2025 \
	$(HIGHRES_DIR)/zonedbtesting

# Highres target. Since this is the default, use 'acetimec.a' instead of the
# more correct 'acetimech.a', if we followed the naming scheme. Using the old
# name allows for backwards compatibility.
acetimec.a: $(HIGHRES_OBJS)
	$(AR) -c -v -r $@ $(HIGHRES_OBJS)

$(HIGHRES_DIR)/%.o: %.c $(HIGHRES_HEADERS) $(HIGHRES_DIR_PROXY)
	$(CC) -c $(CFLAGS) $(HIGHRES_FLAGS) -o $@ $<

# The touch file is a proxy for the various build directories. If we try to
# depend directly on the directories, 'make' seems to rebuild everything all
# the time.
$(HIGHRES_DIR_PROXY):
	@touch $@
	@mkdir -p $(HIGHRES_BUILD_DIRS)

#------------------------------------------------------------------------------
# Midres files. Experimental.
#------------------------------------------------------------------------------

# Project object files

# Midres settings
MIDRES_DIR := midres
MIDRES_DIR_PROXY := midres_dir.touch
MIDRES_BARE_OBJS := $(MIDRES_SRCS:.c=.o)
MIDRES_OBJS := $(addprefix $(MIDRES_DIR)/, $(MIDRES_BARE_OBJS))
MIDRES_FLAGS := -D ACE_TIME_C_ZONEDB_RES=ACE_TIME_C_ZONEDB_RES_MID
MIDRES_BUILD_DIRS := \
	$(MIDRES_DIR)/acetimec \
	$(MIDRES_DIR)/zoneinfo \
	$(MIDRES_DIR)/zonedb2000m \
	$(MIDRES_DIR)/zonedb2025m \
	$(MIDRES_DIR)/zonedbtestingm

# Midres target
acetimecm.a: $(MIDRES_OBJS)
	$(AR) -c -v -r $@ $(MIDRES_OBJS)

$(MIDRES_DIR)/%.o: %.c $(HEADERS) $(MIDRES_DIR_PROXY)
	$(CC) -c $(CFLAGS) $(MIDRES_FLAGS) -o $@ $<

# The touch file is a proxy for the various build directories. If we try to
# depend directly on the directories, 'make' seems to rebuild everything all
# the time.
$(MIDRES_DIR_PROXY):
	@touch $@
	@mkdir -p $(MIDRES_BUILD_DIRS)

#------------------------------------------------------------------------------
# Regenerate zone databases
#------------------------------------------------------------------------------

.PHONY: zonedbs

zonedbs:
	$(MAKE) -C zonedball
	$(MAKE) -C zonedb2000
	$(MAKE) -C zonedb2025
	$(MAKE) -C zonedbtesting
	$(MAKE) -C zonedb2000m
	$(MAKE) -C zonedb2025m
	$(MAKE) -C zonedbtestingm

#------------------------------------------------------------------------------
# Clean up
#------------------------------------------------------------------------------

clean:
	rm -rf $(HIGHRES_DIR) $(HIGHRES_DIR_PROXY) acetimec.a \
		$(MIDRES_DIR) $(MIDRES_DIR_PROXY) acetimecm.a \
