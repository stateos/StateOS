PROJECT := test
DEFS    :=
INCS    := test
SRCS    := test/test.c test/test_resources.c test/test_resources.cpp
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

include test/makefile

#----------------------------------------------------------#
include $(COMMON)/make/makefile
#----------------------------------------------------------#
