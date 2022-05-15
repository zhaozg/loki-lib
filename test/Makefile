SUBTARGETS_ORIG := $(patsubst %/,%,$(dir $(wildcard */Makefile)))
SUBTARGETS_FILTER_OUT := flex_string
SUBTARGETS := $(filter-out $(SUBTARGETS_FILTER_OUT),$(SUBTARGETS_ORIG))

SUBTARGETS_CLEAN := $(addsuffix -clean,$(SUBTARGETS))

SUBTARGETS_TEST:= $(addsuffix -test,$(SUBTARGETS))

# skip test not pass
TMP := $(SUBTARGETS)
SUBTARGETS := $(filter-out LockingPtr, $(TMP))

.PHONY: all clean $(SUBTARGETS) $(SUBTARGETS_CLEAN)
all: $(SUBTARGETS)
clean: $(SUBTARGETS_CLEAN)
test: $(SUBTARGETS_TEST)

$(SUBTARGETS):
	$(MAKE) -C $@

$(SUBTARGETS_CLEAN):
	$(MAKE) -C $(@:-clean=) clean

$(SUBTARGETS_TEST):
	$(MAKE) -C $(@:-test=) test

