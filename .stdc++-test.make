PROJECT := test
GNUCC   :=
DEFS    :=
INCS    := stdc++/include
SRCS    := $(MAIN)
LIBS    :=
SCRIPT  :=
COMMON  := common
#----------------------------------------------------------#
include $(COMMON)/stateos/make/stm32f4discovery/makefile.gnucc
#----------------------------------------------------------#
include $(COMMON)/cmsis/makefile
include $(COMMON)/device/nosys/makefile
include $(COMMON)/startup/makefile
include $(COMMON)/stateos/stdc++/makefile
#----------------------------------------------------------#
include $(COMMON)/make/makefile
#----------------------------------------------------------#
