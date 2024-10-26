#include <stdlib.h>
#include <stdio.h>
#include "mlpt.h"
#include "config.h"
#include <stdalign.h>
#include <string.h>

size_t ptbr; // Definition of ptbr

//NOTE: For makefile, not sure if it's required but I believe the lib should be in .a format

size_t translate(size_t va) {

    if (ptbr == 0) {
        // no page table, return all 1 bits
        return (size_t) ~0ULL;
    }

    // get the offset using bitwise AND
    size_t offset = va & ((1ULL << POBITS) - 1);

    size_t page_table_level_indicies[LEVELS];
    size_t truncated_va = va >> POBITS;

    // determine num bits per index
    size_t bits_per_index = POBITS - 3; // 3 is log2(8)

    // get the page_table_level_indicies using bit shifting
    for (int i = LEVELS - 1; i >= 0; i -= 1) {
        page_table_level_indicies[i] = truncated_va & ((1ULL << POBITS) - 1);
        truncated_va = truncated_va >> bits_per_index;
    }

    // get the page table using the page table base register
    size_t *page_table = (size_t *) ptbr;

    // get the page table entry using the page_table_level_indicies
    for (int i = 0; i < LEVELS; i += 1) {
        size_t page_table_entry = page_table[page_table_level_indicies[i]];

        size_t valid_bit = page_table_entry & 1ULL;
        size_t next_level_address = page_table_entry & ~(1ULL);

        // check for invalid bit
        if (!(valid_bit)) {
            // invalid and return all 1 bits
            return (size_t) ~0ULL;
        }

        // if at last level, return physical address
        if (i == LEVELS - 1) {
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

    if (ptbr == 0) {
        // allocate memory for the page table
        size_t page_table_size = (1ULL << POBITS);
        int ret = posix_memalign((void **)&ptbr, page_table_size, page_table_size);

        // ensure that posix_memalign was successful
        if (ret != 0) {
            fprintf(stderr, "posix_memalign failed\n");
            exit(1);
        }
        
        // set all entries in the page table to 0
        memset((void *) ptbr, 0, page_table_size);
    }

    size_t page_table_level_indicies[LEVELS];
    size_t truncated_va = va >> POBITS;

    // determine num bits per index
    size_t bits_per_index = POBITS - 3; // 3 is log2(8)

    // get the page_table_level_indicies for each level using bit shifting
    for (int i = LEVELS - 1; i >= 0; i -= 1) {
        page_table_level_indicies[i] = truncated_va & ((1ULL << bits_per_index) - 1);
        truncated_va = truncated_va >> bits_per_index;
    }

    // get the page table using the page table base register
    size_t *page_table = (size_t *) ptbr;

    // get the page table entry using the page_table_level_indicies
    for (int level = 0; level < LEVELS; level += 1) {
        size_t page_table_entry = page_table[page_table_level_indicies[level]];

        size_t valid_bit = page_table_entry & 1ULL;
        size_t next_level_address = page_table_entry & ~(1ULL);

        // check if the valid bit is invalid, and allocate memory for the page table
        if (!(valid_bit)) {
            // if only the last level, allocate memory for the PP
            if (level == LEVELS - 1) {
                // define page table size and allocate memory using memalign
                void *physical_page;
                size_t page_table_size = (1ULL << POBITS);
                int ret = posix_memalign(&physical_page, page_table_size, page_table_size);

                // ensure that posix_memalign was successful
                if (ret != 0) {
                    fprintf(stderr, "posix_memalign failed\n");
                    exit(1);
                }

                // get physical page number (PPN) and store as the address of next level
                next_level_address = ((size_t) physical_page);
            }
            // not on last level, allocate memory for the next level page table
            else {
                // define page table size and allocate memory using memalign
                void *next_level_page_table;
                size_t page_table_size = (1ULL << POBITS);
                int ret = posix_memalign(&next_level_page_table, page_table_size, page_table_size);

                // ensure that posix_memalign was successful
                if (ret != 0) {
                    fprintf(stderr, "posix_memalign failed\n");
                    exit(1);
                }

                // initialize the page table entries
                memset(next_level_page_table, 0, page_table_size);
                next_level_address = ((size_t) next_level_page_table);
            }
            // page table entry to be updated
            page_table_entry = next_level_address | 1ULL;
            // set the page table entry in the page table
            page_table[page_table_level_indicies[i]] = page_table_entry;
        }

        // not on last level, get the next level page table
        if (level != LEVELS - 1) {
            page_table = (size_t *) next_level_address;
        }
        // on last level, return
        else {
            return; 
        }
    }
}