#
# Copyright (C) 2021 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Kernel
BOARD_KERNEL_IMAGE_NAME := Image.lz4
KERNEL_LD := LD=ld.lld
TARGET_COMPILE_WITH_MSM_KERNEL := true
TARGET_KERNEL_ADDITIONAL_FLAGS := DTC_EXT=$(shell pwd)/prebuilts/misc/linux-x86/dtc/dtc
TARGET_KERNEL_ARCH := arm64
TARGET_KERNEL_CLANG_COMPILE := true
TARGET_KERNEL_CONFIG := redbull_defconfig
TARGET_KERNEL_SOURCE := kernel/google/redbull
TARGET_NEEDS_DTBOIMAGE := true

# Kernel modules
BOOT_KERNEL_MODULES += \
    heatmap.ko \
    touch_offload.ko \

ifeq ($(TARGET_BOOTLOADER_BOARD_NAME),redfin)
    BOOT_KERNEL_MODULES += sec_touch.ko
else ifeq ($(TARGET_BOOTLOADER_BOARD_NAME),bramble)
    BOOT_KERNEL_MODULES += ftm5.ko
endif

KERNEL_MODULES_LOAD_RAW := $(strip $(shell cat device/google/redbull/modules.load))
KERNEL_MODULES_LOAD := $(foreach m,$(KERNEL_MODULES_LOAD_RAW),$(notdir $(m)))
BOARD_VENDOR_KERNEL_MODULES_LOAD := $(filter-out $(BOOT_KERNEL_MODULES), $(KERNEL_MODULES_LOAD))
BOARD_VENDOR_RAMDISK_KERNEL_MODULES_LOAD := $(filter $(BOOT_KERNEL_MODULES), $(KERNEL_MODULES_LOAD))

# Verified Boot
BOARD_AVB_MAKE_VBMETA_IMAGE_ARGS += --set_hashtree_disabled_flag
BOARD_AVB_MAKE_VBMETA_IMAGE_ARGS += --flags 2
