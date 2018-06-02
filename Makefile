#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := ESP32-LIN

EXTRA_COMPONENT_DIRS += $(PROJECT_PATH)/esp-adf/components/

ADF_VER := $(shell cd esp-adf && git describe --always --tags --dirty)
include $(IDF_PATH)/make/project.mk

