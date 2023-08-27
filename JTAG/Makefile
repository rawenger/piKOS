PREFIX	= aarch64-none-elf-
AS		= $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
KERNEL	= kernel8
ASFLAGS = -nostdlib -nostartfiles -ffreestanding
ASFLAGS += -Wl,--section-start=.text.boot=0x80000

HOST = $(strip $(shell uname -s))
ifeq ($(HOST), Linux)
	SDCARD_PATH ?= /run/media/$(shell whoami)/boot
else
	ifeq ($(HOST), Darwin)
		SDCARD_PATH ?= /Volumes/BOOT/
	endif
endif

all: $(KERNEL).img

install: all
	@echo "  INSTALL $(SDCARD_PATH)"
	@cp $(KERNEL).img $(SDCARD_PATH)

$(KERNEL).img: $(KERNEL).elf
	@echo "  COPY    $(KERNEL).img"
	@$(OBJCOPY) $< -O binary $@

$(KERNEL).elf: not-boot.S
	@echo "  AS      $<"
	@echo "  LD      $@"
	@$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(KERNEL).elf $(KERNEL).img

.PHONY: clean all
