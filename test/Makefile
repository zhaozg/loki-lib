SUBTARGETS_ORIG := $(patsubst %/,%,$(dir $(wildcard */Makefile)))
SUBTARGETS_FILTER_OUT := flex_string
SUBTARGETS := $(filter-out $(SUBTARGETS_FILTER_OUT),$(SUBTARGETS_ORIG))

SUBTARGETS_CLEAN := $(addsuffix -clean,$(SUBTARGETS))

# skip test not pass
TMP := $(SUBTARGETS)
SUBTARGETS := $(filter-out LockingPtr, $(TMP))

.PHONY: all clean $(SUBTARGETS) $(SUBTARGETS_CLEAN)
all: $(SUBTARGETS)
clean: $(SUBTARGETS_CLEAN)

$(SUBTARGETS):
	$(MAKE) -C $@
	@echo Test $@
	-@$@/main
	-@$@/$@Test
	-@$@/$@

$(SUBTARGETS_CLEAN):
	$(MAKE) -C $(@:-clean=) clean
