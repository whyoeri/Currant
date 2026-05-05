#include "multiboot.h"

#define MULTIBOOT_INFO_MEMORY_MAP (1 << 6)

uint32_t get_max_address(multiboot_info_t* mbi){
    if(!(mbi->flags & MULTIBOOT_INFO_MEMORY_MAP)){return 0;}

    mmap_multiboot_info_t* mmap_mbi = (mmap_multiboot_info_t*)mbi->mmap_addr;

    uint32_t max_addr = 0;

    while((uint32_t)mmap_mbi < mbi->mmap_addr + mbi->mmap_len){
        if(1 == mmap_mbi->type){
            uint32_t region_end = (uint32_t)mmap_mbi->addr + (uint32_t)mmap_mbi->len;
            if(region_end > max_addr){
                max_addr = region_end;
            }
        }
        mmap_mbi = (mmap_multiboot_info_t*)((uint32_t)mmap_mbi + mmap_mbi->size + sizeof(mmap_mbi->size));
    }
    return max_addr;
}