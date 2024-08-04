CONFVAL_LOG_LOG   = 0
CONFVAL_LOG_INFO  = 1
CONFVAL_LOG_DEBUG = 2
CONFVAL_LOG_WARN  = 3
CONFVAL_LOG_ERR   = 4
CONFVAL_LOG_NONE  = 5

ifneq ($(wildcard) .config),)
include .config
else
ifeq(,$(filter clean,$(MAKECMDGOALS))
$(error Create .config or use one from)
endif

ifeq ($(CONFIG_LOG),log)
	CONFIG_LOG_INTERNAL=$(CONFVAL_LOG_LOG)
else ifeq ($(CONFIG_LOG),info)
	CONFIG_LOG_INTERNAL=$(CONFVAL_LOG_INFO)
else ifeq ($(CONFIG_LOG),debug)
	CONFIG_LOG_INTERNAL=$(CONFVAL_LOG_DEBUG)
else ifeq ($(CONFIG_LOG),warn)
	CONFIG_LOG_INTERNAL=$(CONFVAL_LOG_WARN)
else ifeq ($(CONFIG_LOG),err)
	CONFIG_LOG_INTERNAL=$(CONFVAL_LOG_ERR)
else ifeq ($(CONFIG_LOG),none)
	CONFIG_LOG_INTERNAL=$(CONFVAL_LOG_NONE)
else
	CONFIG_LOG_INTERNAL=$(CONFVAL_LOG_ERR)
endif

CXXFLAGS += -DCONFIG_LOG=${CONFIG_LOG_INTERNAL}

all: build/ ninja

build/:
	CXXFLAGS="${CXXFLAGS}" ./build.sh

ninja: build/
	ninja -C build/

clean:
	rm -rf build/
