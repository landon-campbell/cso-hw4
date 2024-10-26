## Multi-Level Page Table Simulation
* Simluates multi-level page table lookups, allocation, and virtual address translation.
--

# Customization of config.h
In this file are two modifieable parameters:

* LEVELS: The number of levels in the page table
- Adjusts the number of levels in the page table hierarchy. This also works for one-level page table.
- By increasing the number of levels, the model can simulate a more complex page table structure, and use larger virtual address spaces.
- Each level has a corresponding number of bits in the virtual address that are used to index into that level of the page table.
- It is important to note that LEVELS * number of index bits + POBITS mut not exceed sizeof(size_t)

* POBITS: The number of bits in the page offset
- Determines the page size; the page size is 2^POBITS bytes.
- Common page sizes are 4KB (12 bits), 2MB (21 bits), and 1GB (30 bits).
- POBITS adjusts the memory allocation refinement.

# Example Configuration

* Virtual Address Space of 64 bits
- Use LEVELS = 4, POBITS = 12

* Virtual Address Space of 32 bits
- Use LEVELS = 2, POBITS = 12

# Usage
Include mlpt.h in your code and call the functions as needed, link with libmlpt.a.

* Functionality
- void page_allocate(size_t va): Allocates a page for the given virtual address.
- void translate(size_t va): Translates the virtual address to a physical address, returns ~0ULL if the address could not be allocated.

# Limitations
- The models does not suppose dynamic page sizes, and it is determined by POBITS at compile time.
- No bigger than 64 bit virtual addresses

# Big-O Analysis
* Time Complexity of page_allocate is O(n) where n is the number of levels in the page table.
* Space Complexity of page_allocate, for every allocation is one physical page, and n page tables, where n is the number of levels in the page table.
* Time Complexity of translate is O(n) where n is the number of levels in the page table. In order to translate a virtual address, the model must traverse the page table from top to bottom
* Space Complexity of translate is n/a as it does not allocate any memory directly.

# Code Examples of Usage
```c
#include "mlpt.h"
#include <stdio.h>

int main() {
    size_t va = 0x12345000; // Example virtual address

    size_t pa = translate(va);
    if (pa == (size_t)~0ULL) {
        printf("The address has not been allocated. Will allocate now.\n");
        page_allocate(va);
    }

    pa = translate(va);
    if (pa != (size_t)~0ULL) {
        printf("Virtual Address: 0x%zx => Physical Address: 0x%zx\n", va, pa);
    } else {
        printf("The V.A. translation failed.\n");
    }

    return 0;
}
```

# Regarding De-allocation
- It is not possible to currently implement a deallocation method.

* To support de-allocation, the interface would need to change:
- Include some form of counter and reference to the valid page table entries.
- Modify page_allocate() to maintain reference counts at the time of allocation.
- Modify translate() method of reading page table entries to account for any interface/format differences of page table entries.
- Implement a recursive method, void page_deallocate(size_t va) that traversed the entire page table tree from bottom to top and dealloate everything.
This method would deallocate the page mapped to the virtual address va. It would also remove any mapping from the page tables, and free all page tables, including physical page.