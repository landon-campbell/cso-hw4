// test_mlpt.c
#include "mlpt.h"
#include <stdio.h>

int main() {
    size_t va = 0x12345000; // Example virtual address

    size_t pa = translate(va);
    if (pa == (size_t)~0ULL) {
        printf("Address not allocated. Allocating now.\n");
        page_allocate(va);
    }

    pa = translate(va);
    if (pa != (size_t)~0ULL) {
        printf("Virtual Address: 0x%zx => Physical Address: 0x%zx\n", va, pa);
    } else {
        printf("Translation failed.\n");
    }

    return 0;
}
