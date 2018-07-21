#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
ADF_PATH:=$(LIN_PATH)esp-adf
EXTRA_COMPONENT_DIRS += $(ADF_PATH)/components/ $(LIN_PATH)components/

ADF_VER := $(shell cd ${ADF_PATH} && git describe --always --tags --dirty)
include $(IDF_PATH)/make/project.mk

