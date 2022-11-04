all:
	set -e; \
	for i in src/Makefile tests/Makefile; do \
		echo '==== Making:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i); \
	done

runtests:
	$(MAKE) -C tests runtests

clean:
	set -e; \
	for i in src/Makefile tests/Makefile; do \
		echo '==== Cleaning:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) clean; \
	done
