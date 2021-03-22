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
include $(COMMON)/stateos/nasa/makefile
include $(COMMON)/stateos/cmsis/makefile
#----------------------------------------------------------#
include test/makefile
#----------------------------------------------------------#
include $(COMMON)/make/makefile
#----------------------------------------------------------#
