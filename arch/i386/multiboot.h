#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include "lib/other/types.h"

typedef struct __attribute__((packed)){
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
    uint32_t mmap_len;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
} multiboot_info_t;

typedef struct __attribute__((packed)){
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} mmap_multiboot_info_t;

uint32_t get_max_address(multiboot_info_t* mbi);

#endif