PREFIX	= aarch64-none-elf-
AS		= $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
KERNEL	= kernel8
ASFLAGS = -nostdlib -nostartfiles -ffreestanding
ASFLAGS += -Wl,--section-start=.text.boot=0x80000

all: $(KERNEL).img

$(KERNEL).img: $(KERNEL).elf
	@echo "  COPY  $(KERNEL).img"
	@$(OBJCOPY) $< -O binary $@

$(KERNEL).elf: not-boot.S
	@echo "  AS    $<"
	@echo "  LD    $@"
	@$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(KERNEL).elf $(KERNEL).img

.PHONY: clean all
