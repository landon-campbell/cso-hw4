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
}