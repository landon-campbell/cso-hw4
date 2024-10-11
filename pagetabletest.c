// pagetabletest.c
#include <stdalign.h>
#include <stdio.h>
#include "mlpt.h"
#include "config.h"

int main() {
    alignas(4096)
    static size_t testing_page_table[512];

    alignas(4096)
    static char data_for_page_3[4096];
    size_t address_of_data_for_page_3_as_integer = (size_t) &data_for_page_3[0];
    size_t physical_page_number_of_data_for_page_3 = address_of_data_for_page_3_as_integer >> POBITS;

    // Set up a page table entry with valid bit set to 1 for page number 3
    size_t page_table_entry_for_page_3 = (
        (physical_page_number_of_data_for_page_3 << POBITS) | 1
    );
    // Store the entry in the page table
    testing_page_table[3] = page_table_entry_for_page_3;

    // Set ptbr to point to testing_page_table
    ptbr = (size_t) testing_page_table;

    // Test with a valid virtual address (valid bit is 1)
    size_t translated_address = translate(0x3045);
    size_t expected_address = (size_t) &data_for_page_3[0x45];

    printf("Test with valid bit set to 1:\n");
    printf("translate(0x3045) == &data_for_page_3[0x45]: %s\n",
           (translated_address == expected_address) ? "PASS" : "FAIL");

    // Now, test with a virtual address where the valid bit is 0
    // Let's set the valid bit to 0 for page table entry at index 4
    // We still assign a physical page number, but the valid bit is 0
    size_t page_table_entry_for_page_4 = (
        (physical_page_number_of_data_for_page_3 << POBITS) & ~((size_t)1)
    );
    // Store the entry in the page table
    testing_page_table[4] = page_table_entry_for_page_4;

    // Test with a virtual address that maps to page table index 4
    size_t invalid_translated_address = translate(0x4000); // 0x4000 maps to page number 4
    size_t expected_invalid_address = (size_t) ~0L;

    printf("Test with valid bit set to 0:\n");
    printf("translate(0x4000) == ~0L: %s\n",
           (invalid_translated_address == expected_invalid_address) ? "PASS" : "FAIL");

    return 0;
}
