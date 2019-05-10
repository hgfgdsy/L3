NAME           := kernel
SRCS           := framework/main.c $(shell find -L ./src/ -name "*.c" -o -name "*.S")
INC_DIR        := include/ framework/
export AM_HOME := $(PWD)/../abstract-machine
ifeq ($(ARCH),)
export ARCH := x86-qemu
endif

PREBUILD := git initrd
include $(AM_HOME)/Makefile.app
include ../Makefile.lab

.PHONY: initrd

initrd:
	@seq 1 8192 > build/ramdisk.img

QEMU_FLAGS := -serial stdio -machine accel=kvm:tcg -drive format=raw,file=$(BINARY)

run1: image
	qemu-system-i386 -smp 1 $(QEMU_FLAGS)

run2: image
	qemu-system-i386 -smp 2 $(QEMU_FLAGS)

run4: image
	qemu-system-i386 -smp 4 $(QEMU_FLAGS)

gdb: $(IMG)
	@gdb -ex "target remote 127.0.0.1:1234" -ex "symbol $(BINARY).o"

debug: $(IMG)
	qemu-system-i386 -smp 2 -S -s -monitor telnet:127.0.0.1:1111,server,nowait -serial stdio -drive format=raw,file=$(BINARY)

