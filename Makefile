RASPPI ?= 3
AARCH = 64 # for now, 32-bit is unsupported
DEBUG ?= 1 # set to 1 to enable debug info & define DEBUG macro
OPTIMIZE_LEVEL ?= 3 # argument to the compiler's '-O' flag. Only applies to non-debug builds

ifeq ($(strip $(RASPPI)), 3)
	TARGET_CPU  = cortex-a53
	KERNEL 	    = kernel8
	INITADDR    = 0x80000
	QEMU_FLAGS += -M raspi3b -nographic #-no-reboot
	QEMU_FLAGS += -monitor telnet:127.0.0.1:1235,server,nowait
else
	ifeq ($(strip $(RASPPI)), 4)
		TARGET_CPU = cortex-a72
		KERNEL 	   = kernel8-rpi4
		INITADDR   = 0x80000
	else # not supported
		TARGET_CPU =
		KERNEL 	   = kernel7
	endif
endif

$(info Kernel: $(KERNEL))
$(info Debug: $(DEBUG))

HOSTCPU=$(shell uname -m)
ifeq ($(strip $(HOSTCPU)), $(filter $(HOSTCPU), aarch64 arm64))
	ifeq ($(strip $(HOSTCPU)), arm64) #macOS
		QEMU_FLAGS += -accel hvf
	else # aarch64 Linux can use native host toolchain
		PREFIX     ?=
		CFLAGS 	   += -fno-stack-protector
		QEMU_FLAGS += -accel tcg #-accel kvm # <-- asahi kernel supports this, but it seems Asahi Linux official distro does not, due to lack of ALARM support for the req'd modules
	endif
else # host is almost definitely x86(_64) because otherwise WTF r u running LOL
	PREFIX 	   ?= aarch64-none-elf-
	CFLAGS 	   +=
	QEMU_FLAGS += -accel tcg
endif

CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
AS=$(PREFIX)as
LD=$(PREFIX)ld
OBJCOPY=$(PREFIX)objcopy

C_INCLUDES  = include # quote to distinguish from builtin make directive
AS_INCLUDES = include

CFLAGS  += -Wall -std=gnu17
CFLAGS  += -nostdlib -nostartfiles -ffreestanding
CFLAGS	+= -mcpu=$(TARGET_CPU)
CFLAGS  += -DRASPPI=$(RASPPI) -DAARCH=$(AARCH)
ASFLAGS += # add more here when necessary

LDFLAGS += --section-start=.init=$(INITADDR)

CFLAGS  += $(addprefix -I, $(C_INCLUDES))
ASFLAGS += $(addprefix -I, $(AS_INCLUDES))

ifneq ($(strip $(DEBUG)), 1)
	CFLAGS 		+= -O$(strip $(OPTIMIZE_LEVEL))
else
	CFLAGS		+= -g -Og -DDEBUG
	ASFLAGS		+= -g -DDEBUG
	# enable sending debug messages to stderr over the mini UART
	QEMU_FLAGS	+= -chardev file,path=/dev/stderr,id=uart1 -serial chardev:uart1
endif


SRC_DIR   = src
BUILD_DIR = build

TARGET = $(KERNEL).img

C_FILES    = $(wildcard $(SRC_DIR)/*.c)
C_FILES   += $(wildcard $(SRC_DIR)/util/*.c)
C_FILES   += $(wildcard $(SRC_DIR)/peripherals/*.c)
C_FILES   += $(wildcard $(SRC_DIR)/drivers/*.c)
ASM_FILES  = $(wildcard $(SRC_DIR)/*.S)
ASM_FILES += $(wildcard $(SRC_DIR)/*.s)
ASM_FILES += $(wildcard $(SRC_DIR)/boot/*.S)
ASM_FILES += $(wildcard $(SRC_DIR)/boot/*.s)
ASM_FILES += $(wildcard $(SRC_DIR)/util/*.s)
ASM_FILES += $(wildcard $(SRC_DIR)/util/*.S)
OBJ_FILES  = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_S.o)

all: $(TARGET)

DEP_FILES = $(OBJ_FILES:$%.o=%.d)
-include $(DEP_FILES)


$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.s
	@mkdir -p $(@D)
	@echo "  AS    $<"
	@$(AS) $(ASFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%_S.o: $(SRC_DIR)/%.S
	@mkdir -p $(@D)
	@echo "  AS    $<"
	@$(CC) $(ASFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/$(KERNEL).elf: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	@echo "  LD    $(KERNEL).elf"
	@$(LD) $(LDFLAGS) -T $(SRC_DIR)/linker.ld -o $@  $(OBJ_FILES)

$(KERNEL).img: $(BUILD_DIR)/$(KERNEL).elf
	@echo "  COPY  $(KERNEL).img"
	@$(OBJCOPY) $< -O binary $(KERNEL).img

qemu: $(KERNEL).img
	qemu-system-aarch64 -serial stdio $(QEMU_FLAGS) -kernel $<

qemu-gdb: $(KERNEL).img
	@echo "Attach debugger with"
	@echo "$(PREFIX)gdb -ex 'target remote :18427' -ex 'monitor system_reset' $(BUILD_DIR)/$(KERNEL).elf"
	@echo "Attach to QEMU monitor with 'telnet 127.0.0.1 1235'"
	@echo "Attach to serial console with 'telnet 127.0.0.1 1236'"
	qemu-system-aarch64 -serial telnet:127.0.0.1:1236,server $(QEMU_FLAGS) -S -gdb tcp::18427 -kernel $<


clean:
	@echo "  CLEAN $(BUILD_DIR)"
	@echo "  CLEAN $(KERNEL).img"
	@rm -rf $(BUILD_DIR) $(KERNEL).img


.PHONY: qemu qemu-gdb clean all
