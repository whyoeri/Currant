#include "pmm.h"

#define SYSTEM_INFO_SIZE 0x100000
#define PAGE_SIZE 4096
#define BITMAP_FULL 0xFFFFFFFF
#define BITS_IN_BYTE 8
#define BITS_UINT32 32
#define BYTE_UINT32 4

static volatile pmm_control_t pmm;

void init_pmm(multiboot_info_t* mbi){
    uint32_t max_addr = get_max_address(mbi);
    pmm.bitmap = (uint32_t*)&ebss;
    pmm.total_frames = max_addr / PAGE_SIZE;
    pmm.bitmap_size = pmm.total_frames / BITS_IN_BYTE;
    pmm.used_frames = pmm.total_frames;

    for(uint32_t i = 0; i < pmm.bitmap_size / BYTE_UINT32; i++){
        pmm.bitmap[i] = BITMAP_FULL;
    }
    
    mmap_multiboot_info_t* mmap_mbi = (mmap_multiboot_info_t*)mbi->mmap_addr;
    while((uint32_t)mmap_mbi < mbi->mmap_addr + mbi->mmap_len){
        if(1 == mmap_mbi->type){
            init_pmm_region((uint32_t)mmap_mbi->addr, (uint32_t)mmap_mbi->len);
        }
        mmap_mbi = (mmap_multiboot_info_t*)((uint32_t)mmap_mbi + mmap_mbi->size + BYTE_UINT32);
    }
    
    uint32_t end_kernel = (uint32_t)&ebss + pmm.bitmap_size;
    lock_pmm_region(0, end_kernel);
    set_frame_pmm(0);
}

void set_frame_pmm(uint32_t addr){
    uint32_t frame = addr / PAGE_SIZE;
    pmm.bitmap[frame / BITS_UINT32] |= (1 << (frame % BITS_UINT32));
}

void unset_frame_pmm(uint32_t addr){
    uint32_t frame = addr / PAGE_SIZE;
    pmm.bitmap[frame / BITS_UINT32] &= ~(1 << (frame % BITS_UINT32));
}

void init_pmm_region(uint32_t addr, uint32_t len){
    for(uint64_t a = addr; a < addr + len; a += PAGE_SIZE){
        unset_frame_pmm(a);
        if(pmm.used_frames > 0){pmm.used_frames--;}
    }
}

void lock_pmm_region(uint32_t addr, uint32_t len){
    for(uint32_t a = addr; a < addr + len; a += PAGE_SIZE){
        set_frame_pmm(a);
        pmm.used_frames++;
    }
}

uint32_t get_end_kernel_area(void){
    return (uint32_t)((uint32_t)&ebss + pmm.bitmap_size);
}

uint32_t alloc_pmm(void){
    for(uint32_t i = 0; i < pmm.bitmap_size / BYTE_UINT32; i++){
        if(pmm.bitmap[i] != BITMAP_FULL){
            for(uint32_t j = 0; j < BITS_UINT32; j++){
                if(!(pmm.bitmap[i] & (1 << j))){
                    uint64_t addr = (i * BITS_UINT32 + j) * PAGE_SIZE;
                    set_frame_pmm(addr);
                    pmm.used_frames++;
                    return addr;
                }
            }
        }
    }
    return 0;
}