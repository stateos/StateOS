PROJECT := test
GNUCC   :=
DEFS    :=
INCS    := examples/include
SRCS    := $(wildcard examples/main.*)
LIBS    :=
SCRIPT  :=
COMMON  := common

#----------------------------------------------------------#
include $(COMMON)/stateos/make/stm32f4discovery/makefile.gnucc
#----------------------------------------------------------#

include $(COMMON)/cmsis/makefile
include $(COMMON)/device/nosys/makefile
include $(COMMON)/startup/makefile
include $(COMMON)/stateos/makefile

#----------------------------------------------------------#
include $(COMMON)/make/makefile
#----------------------------------------------------------#
