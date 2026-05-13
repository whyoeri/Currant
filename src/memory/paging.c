#include "paging.h"

#include "src/memory/pmm.h"

#define CRO_PAGING_ENABLE 0x80000000
#define SIZE_TABLE 4096
#define KERNEL_PD_FIRST_INDEX 0
#define RECURSIVE_MAPPING_INDEX 1023

volatile page_directory_t* kernel_pd = NULL;

void init_paging(void){
    kernel_pd = (page_directory_t*)alloc_pmm();
    page_table_t* first_pt = (page_table_t*)alloc_pmm(); 

    for(int i = 0; i < COUNT_ENTRIES_PER_PAGE; i++){
        kernel_pd->pe[i] = 0;
        first_pt->pe[i] = 0;
    }
    
    for(int i = 0; i < COUNT_ENTRIES_PER_PAGE; i++){
        first_pt->pe[i] = (i * SIZE_TABLE) | PAGE_PRESENT | PAGE_RW | PAGE_US;
    }
    
    kernel_pd->pe[KERNEL_PD_FIRST_INDEX] = ((uint32_t)first_pt) | PAGE_PRESENT | PAGE_RW | PAGE_US;
    kernel_pd->pe[RECURSIVE_MAPPING_INDEX] = ((uint32_t)kernel_pd) | PAGE_PRESENT | PAGE_RW;

    load_page_directory((uint32_t*)kernel_pd);
    enable_paging();
}

void load_page_directory(volatile uint32_t* directory){
    __asm__ volatile("mov %0, %%cr3" : : "r"(directory));
}

void enable_paging(void){
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= CRO_PAGING_ENABLE;
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}