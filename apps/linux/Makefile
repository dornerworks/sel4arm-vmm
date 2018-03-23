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

ifeq ($(CONFIG_PLAT_ZYNQMP),y)

export ARCH=arm64

LINUX_BINARY := $(ROOT_DIR)/Image
DEVICE_TREE_SRC := $(ROOT_DIR)/system-top.dts
DEVICE_TREE_SRC := $(ROOT_DIR)/linux_zynqmp.dts

else
$(error Unknown platform)
endif

linux: $(SEL4_BINDIR)/linux-dtb $(SEL4_BINDIR)/linux

$(SEL4_BINDIR)/linux-dtb: $(DEVICE_TREE_SRC)
	$(Q)mkdir -p $(dir $@)
	$(Q)which dtc && dtc -I dts -O dtb $(DEVICE_TREE_SRC) > $@ || \
	(echo "ERROR: dtc potentially not installed" && false)

$(SEL4_BINDIR)/linux: $(LINUX_BINARY)
	$(Q)mkdir -p $(dir $@)
	cp $(LINUX_BINARY) $@

all: linux

.PHONY: linux

.FORCE:
