#include <stdlib.h>
#include <stdio.h>
#include "mlpt.h"
#include "config.h"
#include <stdalign.h>

size_t ptbr; // Definition of ptbr

size_t translate(size_t va){

    if (ptbr == 0){
        // no page table, return all 1 bits
        return (size_t) ~0ULL;
    }

    // get the offset using bitwise AND
    size_t offset = va & ((1ULL << POBITS) - 1);

    size_t indicies[LEVELS];
    size_t truncated_va = va >> POBITS;

    // get the indicies using bit shifting
    for (int i = LEVELS - 1; i >= 0; i--){
        indicies[i] = truncated_va & ((1ULL << POBITS) - 1);
        truncated_va = truncated_va >> POBITS;
    }

    // get the page table using the page table base register
    size_t *page_table = (size_t *) ptbr;

    // get the page table entry using the indicies
    for (int i = 0; i < LEVELS; i++){
        size_t page_table_entry = page_table[indicies[i]];

        size_t flg = page_table_entry & ((1ULL << POBITS) - 1);
        size_t next_level_address = page_table_entry & ~((1ULL << POBITS) - 1);

        // check for invalid bit
        if (!(flg & 1ULL)){
            // invalid and return all 1 bits
            return (size_t) ~0ULL;
        }

        // if at last level, return physical address
        if (i == LEVELS - 1){
            // get the physical address using bit shifting and bitwise OR
            return (size_t) next_level_address | offset;
        }
        // not on last level, and should get the next level page table
        else {
            page_table = (size_t *) next_level_address;
        }
    }
    
    // if something went wrong, return all 1 bits
    return (size_t) ~0ULL;
}

void page_allocate(size_t va){

    if (ptbr == 0){
        // allocate memory for the page table
        size_t page_table_size = (1 << POBITS) * sizeof(size_t);
        int ret = posix_memalign((void **)&ptbr, (1 << POBITS), page_table_size);

        // ensure that posix_memalign was successful
        if (ret != 0){
            fprintf(stderr, "posix_memalign failed\n");
            exit(1);
        }
        
        // set all entries in the page table to 0
        memset((void *) ptbr, 0, page_table_size);
    }

    // get the page number using bit shifting
    size_t page_number = va >> POBITS;
    size_t offset = va & ((1 << POBITS) - 1);

    // get the page table index using bitwise AND
    size_t page_table_index = page_number & ((1 << (LEVELS * POBITS)) - 1);

    size_t *page_table = (size_t *) ptbr;
    // get the page table entry using the page table index
    size_t page_table_entry = page_table[page_table_index];

    // check if the valid bit is 1
    if (page_table_entry & 1){
        // page is already allocated
        return;
    }

    // allocate memory for the page
    size_t physical_page_size = (1 << POBITS);
    void *physical_page;
    int ret = posix_memalign(&physical_page, (1 << POBITS), physical_page_size);

    // ensure that posix_memalign was successful
    if (ret != 0){
        fprintf(stderr, "posix_memalign failed\n");
        exit(1);
    }

    // get physical page number (PPN)
    size_t physical_page_number = ((size_t) physical_page) >> POBITS;

    // set the page table entry
    page_table_entry = (physical_page_number << POBITS) | 1;

    // set the page table entry in the page table
    page_table[page_table_index] = page_table_entry;
}