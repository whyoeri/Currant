#include "vmm.h"

#include "src/memory/pmm.h"
#include "src/memory/paging.h"

#define ADDRESS_PAGE_DIRECTORY 0xFFFFF000
#define ADDRESS_PAGE_TABLE 0xFFC00000
#define SIZE_TABLE 4096
#define PAGE_DIRECTORY_INDEX 22
#define PAGE_TABLE_INDEX 12
#define MASK_INDEX_PAGING 0x3FF
#define COUNT_ENTRIES_PER_PAGE 1024

extern volatile page_directory_t* kernel_pd;

void vmm_map_page(uint32_t vaddr, uint32_t paddr, uint32_t flags){
    uint32_t pd_idx = vaddr >> PAGE_DIRECTORY_INDEX;
    uint32_t pt_idx = (vaddr >> PAGE_TABLE_INDEX) & MASK_INDEX_PAGING;

    uint32_t* magic_pd = (uint32_t*)ADDRESS_PAGE_DIRECTORY;
    uint32_t* magic_pt = (uint32_t*)(ADDRESS_PAGE_TABLE + (pd_idx * SIZE_TABLE));

    if(!(magic_pd[pd_idx] & PAGE_PRESENT)){
        uint32_t new_table = alloc_pmm();
    
        magic_pd[pd_idx] = new_table | PAGE_PRESENT | PAGE_RW;

        for(int i = 0; i < COUNT_ENTRIES_PER_PAGE; i++){
            magic_pt[i] = 0;
        }
    }

    magic_pt[pt_idx] = (paddr & PAGE_FRAME) | flags | PAGE_PRESENT;

    __asm__ volatile("invlpg (%0)" : : "r"(vaddr) : "memory");
}