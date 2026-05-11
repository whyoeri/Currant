AS = nasm
CC = gcc
LD = ld

BUILD_DIR = build
ISO_DIR = isodir
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pie -Wall -O2 -std=gnu11 -Iinclude -I. -I./lib -mno-sse -mno-sse2 -mno-mmx -mno-80387
LDFLAGS = -m elf_i386 -T linker.ld

OBJ = $(BUILD_DIR)/boot.o \
	$(BUILD_DIR)/multiboot.o \
    $(BUILD_DIR)/kernel.o \
    $(BUILD_DIR)/gdt.o \
    $(BUILD_DIR)/gdt_flush.o \
    $(BUILD_DIR)/idt.o \
	$(BUILD_DIR)/idt_flush.o \
	$(BUILD_DIR)/interrupts.o \
    $(BUILD_DIR)/isr.o \
	$(BUILD_DIR)/pmm.o \
	$(BUILD_DIR)/vmm.o \
	$(BUILD_DIR)/paging.o \
	$(BUILD_DIR)/kmalloc.o \
	$(BUILD_DIR)/timer.o \
	$(BUILD_DIR)/switch_context.o \
	$(BUILD_DIR)/scheduler.o \
	$(BUILD_DIR)/tss.o \
    $(BUILD_DIR)/vga.o \
    $(BUILD_DIR)/keyboard.o \
	$(BUILD_DIR)/terminal.o \
	$(BUILD_DIR)/commands.o \
	$(BUILD_DIR)/string.o \
	$(BUILD_DIR)/print.o \
	$(BUILD_DIR)/ringbuffer.o \
    $(BUILD_DIR)/statistics.o \
	$(BUILD_DIR)/kernel_panic.o \
	$(BUILD_DIR)/error_msg_isr.o \

all: prepare_iso

$(BUILD_DIR)/currant.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

prepare_iso: $(BUILD_DIR)/currant.bin
	mkdir -p $(GRUB_DIR)
	cp $(BUILD_DIR)/currant.bin $(BOOT_DIR)/currant.bin
	cp grub.cfg $(GRUB_DIR)/grub.cfg

vpath %.c kernel:arch/i386:src/drivers:src/terminal:src/memory:src/scheduler:lib/data_structures:lib/string:lib/print:lib/statistics:lib/other
vpath %.asm arch/i386

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.asm | $(BUILD_DIR)
	$(AS) -f elf32 $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO_DIR)