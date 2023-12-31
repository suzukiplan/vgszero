CIRCLEHOME = ./circle
CHECK_DEPS = 0
STANDARD = -std=c++17
CFLAGS = -O3
CPPFLAGS = $(CFLAGS)
CPPFLAGS += -D_TIME_T_DECLARED
OBJS = main.o
OBJS += std.o
OBJS += kernel.o
LIBS = $(CIRCLEHOME)/lib/libcircle.a
LIBS += $(CIRCLEHOME)/lib/sched/libsched.a
LIBS += $(CIRCLEHOME)/lib/input/libinput.a
LIBS += $(CIRCLEHOME)/lib/usb/libusb.a
LIBS += $(CIRCLEHOME)/lib/fs/libfs.a
include $(CIRCLEHOME)/Rules.mk
-include $(DEPS)
