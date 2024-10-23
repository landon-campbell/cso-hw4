#include <stdlib.h>
#include <stdio.h>
#include "mlpt.h"
#include "config.h"
#include <stdalign.h>

size_t ptbr; // Definition of ptbr

size_t translate(size_t va){

    // get the page number using bit shifting
    size_t page_number = va >> POBITS;

    // get the offset using bitwise AND
    size_t offset = va & ((1 << POBITS) - 1);

    // get the page table index using bitwise AND
    size_t page_table_index = page_number & ((1 << (LEVELS * POBITS)) - 1);

    // get the page table entry using the page table index
    size_t page_table_entry = ((size_t *) ptbr)[page_table_index];

    // get the physical page number using bit shifting
    size_t physical_page_number = page_table_entry >> POBITS;

    // get the physical address using bit shifting and bitwise OR
    size_t physical_address = (physical_page_number << POBITS) | offset;

    // check if the valid bit is 1
    if (page_table_entry & 1){
        // return physical address is valid bit is 1
        return physical_address;
    }

    // valid bit is 0
    return (size_t) ~0L;
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