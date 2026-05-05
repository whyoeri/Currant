#include "kmalloc.h"

#include "src/memory/paging.h" 
#include "src/memory/vmm.h"
#include "src/memory/pmm.h"
#include "lib/string/string.h"

#define START_VIRTUAL_HEAP_ADDRESS 0xD0000000 
#define PAGE_SIZE 4096
#define ALIG 4
#define MIN_BLOCK_PAYLOAD 4

static kmallfree* head = NULL;
static uint32_t head_top = START_VIRTUAL_HEAP_ADDRESS; 

void* kmalloc(size_t size){
    if(0 == size){return NULL;}
    size = aligment(size, ALIG);
    kmallfree* current = head;
    while(NULL != current){
        if(current->is_free && current->size >= size){
            if(current->size >= size + sizeof(kmallfree) + MIN_BLOCK_PAYLOAD){
                kmallfree* next_block = (kmallfree*)((uint8_t*)current + sizeof(kmallfree) + size);
                next_block->size = current->size - size - sizeof(kmallfree);
                next_block->is_free = 1;
                next_block->next = current->next;
 
                current->size = size;
                current->next = next_block;
            }
            current->is_free = 0;
            return (void*)((uint8_t*)current + sizeof(kmallfree));
        }

        if(NULL == current->next){break;}

        current = current->next;
    }

    uint32_t phys = alloc_pmm();
    if(0 == phys){return NULL;}

    vmm_map_page(head_top, phys, PAGE_PRESENT | PAGE_RW);

    kmallfree* new = (kmallfree*)head_top;
    new->size = PAGE_SIZE - sizeof(kmallfree);
    new->is_free = 1;
    new->next = NULL;

    if(NULL == head){head = new;}
    else{current->next = new;}

    head_top += PAGE_SIZE;

    if(new->size >= sizeof(kmallfree) + size + MIN_BLOCK_PAYLOAD){
        kmallfree* next_block = (kmallfree*)((uint8_t*)new + sizeof(kmallfree) + size);
        next_block->size = new->size - size - sizeof(kmallfree);
        next_block->is_free = 1;
        next_block->next = new->next;

        new->size = size;
        new->next = next_block;
    }

    new->is_free = 0;
    return (void*)((uint8_t*)new + sizeof(kmallfree));
}

void* kcalloc(size_t num, size_t size){
    if(0 == num || 0 == size){return NULL;}
    if(0 != num && size > SIZE_MAX / num){return NULL;}

    size_t total = num * size;
    void* ptr = kmalloc(total);

    if(NULL == ptr){return NULL;}
    memset(ptr, 0, total);
    return ptr;
}

void kfree(void* ptr){
    if(NULL == ptr){return;}
    kmallfree* delete_block = (kmallfree*)((uint8_t*)ptr - sizeof(kmallfree));
    delete_block->is_free = 1;

    if(NULL != delete_block->next && delete_block->next->is_free){
        delete_block->size += sizeof(kmallfree) + delete_block->next->size;
        delete_block->next = delete_block->next->next;
    }
    
    if(delete_block == head){return;}

    kmallfree* current = head;
    
    while(NULL != current && current->next != delete_block){
        current = current->next;
    }

    if(NULL != current && current->is_free){
        current->size += sizeof(kmallfree) + delete_block->size;
        current->next = delete_block->next;
    }
}

size_t aligment(size_t size, size_t alig){
    return (size + alig - 1) & ~(alig - 1);
}