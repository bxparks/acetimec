TARGETS := zone_infos.c zone_infos.h zone_policies.c zone_policies.h

TOOLS := $(abspath ../../../../AceTimeTools)

TZ_VERSION := 2022d
START_YEAR := 2000
UNTIL_YEAR := 10000

all:
	$(TOOLS)/tzcompiler.sh \
		--tag $(TZ_VERSION) \
		--action zonedb \
		--language c \
		--scope extended \
		--generate_int16_years \
		--start_year $(START_YEAR) \
		--until_year $(UNTIL_YEAR) \
		--nocompress

zonedb.json:
	$(TOOLS)/tzcompiler.sh \
		--tag $(TZ_VERSION) \
		--action zonedb \
		--language json \
		--scope extended \
		--generate_int16_years \
		--start_year $(START_YEAR) \
		--until_year $(UNTIL_YEAR)
