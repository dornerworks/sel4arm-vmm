#
# Copyright 2018, DornerWorks
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(NICTA_BSD)
#

SEL4_BINDIR := $(STAGE_DIR)/bin
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

ifneq "$(or $(CONFIG_PLAT_ZYNQMP),$(CONFIG_PLAT_IMX8))" ""

export ARCH=arm64

LINUX_BINARY := $(ROOT_DIR)/$(PLAT)/Image
DEVICE_TREES := $(wildcard $(ROOT_DIR)/$(PLAT)/*.dts)
LINUX_DTBS := $(patsubst %.dts,$(SEL4_BINDIR)/%-dtb,$(patsubst $(ROOT_DIR)/%,%,$(DEVICE_TREES)))

else
$(error Unknown platform)
endif

linux: $(LINUX_DTBS) $(SEL4_BINDIR)/linux

$(LINUX_DTBS): $(DEVICE_TREES)
	$(Q)mkdir -p $(dir $@)
	$(Q)which dtc && dtc -I dts -O dtb $(patsubst $(SEL4_BINDIR)/%-dtb,$(ROOT_DIR)/%.dts,$@) > $@ || \
	(echo "ERROR: dtc potentially not installed" && false)

$(SEL4_BINDIR)/linux: $(LINUX_BINARY)
	$(Q)mkdir -p $(dir $@)
	cp $(LINUX_BINARY) $@

all: linux

.PHONY: linux

.FORCE:
