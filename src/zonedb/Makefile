TARGETS := \
zone_infos.c \
zone_infos.h \
zone_policies.c \
zone_policies.h \
zone_registry.c \
zone_registry.h

TOOLS := $(abspath ../../../AceTimeTools)
TZ_REPO := $(abspath $(TOOLS)/../tz)
TZ_VERSION := 2023c
START_YEAR := 2000
UNTIL_YEAR := 2200

all:
	$(TOOLS)/tzcompiler.sh \
		--tzrepo $(TZ_REPO) \
		--tag $(TZ_VERSION) \
		--action zonedb \
		--language c \
		--scope complete \
		--db_namespace Atc \
		--start_year $(START_YEAR) \
		--until_year $(UNTIL_YEAR) \
		--nocompress

zonedb.json:
	$(TOOLS)/tzcompiler.sh \
		--tzrepo $(TZ_REPO) \
		--tag $(TZ_VERSION) \
		--action zonedb \
		--language json \
		--scope complete \
		--start_year $(START_YEAR) \
		--until_year $(UNTIL_YEAR)

clean:
	rm -rf tzfiles zonedb.json
