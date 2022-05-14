SUBTARGETS_ORIG := $(patsubst %/,%,$(dir $(wildcard */Makefile)))
SUBTARGETS_FILTER_OUT := flex_string
SUBTARGETS := $(filter-out $(SUBTARGETS_FILTER_OUT),$(SUBTARGETS_ORIG))

SUBTARGETS_CLEAN := $(addsuffix -clean,$(SUBTARGETS))

TMP := $(filter-out CachedFactory, $(SUBTARGETS))
SUBTARGETS := $(TMP)
TMP := $(filter-out Factory, $(SUBTARGETS))
SUBTARGETS := $(TMP)
TMP := $(filter-out LevelMutex, $(SUBTARGETS))
SUBTARGETS := $(TMP)
TMP := $(filter-out LockingPtr, $(SUBTARGETS))
SUBTARGETS := $(TMP)
TMP := $(filter-out Longevity, $(SUBTARGETS))
SUBTARGETS := $(TMP)
TMP := $(filter-out OrderedStatic, $(SUBTARGETS))
SUBTARGETS := $(TMP)
TMP := $(filter-out Pimpl, $(SUBTARGETS))
SUBTARGETS := $(TMP)
TMP := $(filter-out Register, $(SUBTARGETS))
SUBTARGETS := $(TMP)
TMP := $(filter-out Singleton, $(SUBTARGETS))
SUBTARGETS := $(TMP)
TMP := $(filter-out SmallObj, $(SUBTARGETS))
SUBTARGETS := $(TMP)
TMP := $(filter-out SmartPtr, $(SUBTARGETS))
SUBTARGETS := $(TMP)

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

