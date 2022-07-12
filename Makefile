include Makefile.common
MFLAGS=
ifneq ($(OS),)
MFLAGS= OS=$(OS)
endif

.PHONY: all build-static build-shared check clean distclean
all: build-static build-shared check
build-static:
	$(MAKE) -C src build-static $(MFLAGS)

build-shared:
	$(MAKE) -C src build-shared $(MFLAGS)

check: build-static build-shared
	$(MAKE) -C test test $(MFLAGS)

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean

distclean: clean
	$(MAKE) -C src distclean

ifneq ($(OS),Windows)
.PHONY: install install-static
install:
	$(MAKE) -C src install-static
	$(MAKE) -C src install-shared
	$(MAKE) -C include install

install-static:
	$(MAKE) -C src install-static
	$(MAKE) -C include install
endif
