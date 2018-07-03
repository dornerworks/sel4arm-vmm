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

IMAGEPREFIX := $(patsubst "%",%,$(CONFIG_APP_LINUX_IMAGE_PREFIX))
LINUX_BINARIES := $(wildcard $(ROOT_DIR)/$(PLAT)/$(IMAGEPREFIX)*)
STAGED_BINARIES := $(subst $(IMAGEPREFIX),$(SEL4_BINDIR)/linux-,$(patsubst $(ROOT_DIR)/$(PLAT)/%,%,$(LINUX_BINARIES)))
DEVICE_TREES := $(wildcard $(ROOT_DIR)/$(PLAT)/*.dts)
LINUX_DTBS := $(patsubst %.dts,$(SEL4_BINDIR)/%-dtb,$(patsubst $(ROOT_DIR)/%,%,$(DEVICE_TREES)))

else
$(error Unknown platform)
endif

linux: $(LINUX_DTBS) $(STAGED_BINARIES)

$(LINUX_DTBS): $(DEVICE_TREES)
	$(Q)mkdir -p $(dir $@)
	$(Q)which dtc && dtc -I dts -O dtb $(patsubst $(SEL4_BINDIR)/%-dtb,$(ROOT_DIR)/%.dts,$@) > $@ || \
	(echo "ERROR: dtc potentially not installed" && false)

$(STAGED_BINARIES): $(LINUX_BINARIES)
	$(Q)mkdir -p $(dir $@)
	cp $(patsubst $(SEL4_BINDIR)/%,$(ROOT_DIR)/$(PLAT)/%,$(subst linux-,$(IMAGEPREFIX),$@)) $@

all: linux

.PHONY: linux

.FORCE:
