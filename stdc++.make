PROJECT := test
GNUCC   :=
DEFS    :=
INCS    :=
SRCS    :=
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
include stdc++/makefile
#----------------------------------------------------------#
include $(COMMON)/make/makefile
#----------------------------------------------------------#
