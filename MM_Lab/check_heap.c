#include "check_heap.h"
#include <stdio.h>
#include <stdint.h>

extern mem_block_header_t *free_head;
extern mem_block_header_t *free_heads[BIN_COUNT];

int check_heap()
{
    for (int i = 0; i < BIN_COUNT; ++i)
    {
        int result = check_bin(free_heads[i]);
        if (result)
            return result;
    }
    return HEAP_SUCCESS;
}

/*
 * STUDENT TODO: set these variables according to your heap design (tests will fail if you do not
 * set these variables!)
 *    - order:      how is your free list ordered?
 *    - circular:   true if your free list is circular; false otherwise
 */
heap_order order = ORD_MEM;
bool circular = false;

/*
 * check_bin -  used to check that the heap is still in a consistent state.
 *
 * STUDENT TODO: this function is required to be completed for checkpoint 1
 *
 *      - Ensure that the free block list is in the order you expect it to be in
 *        (if your list is randomly ordered, this check is not required).
 *
 *      - Check if any free blocks overlap with each other.
 *
 *      - Ensure that each free block is aligned.
 *
 *      - Ensure that all blocks on the free list are free (no implicit free lists)
 *
 * Should return HEAP_SUCCESS if the heap is consistent or HEAP_FAILURE if an error
 * is detected.
 */
int check_bin(mem_block_header_t *free_head)
{

    while (free_head->next != NULL)
    {

        if (free_head > free_head->next)
        {
            return HEAP_FAILURE;
        }

        if ((free_head->block_metadata & 1) == 1)
        {
            return HEAP_FAILURE;
        }

        mem_block_header_t *new_ptr = (free_head + 1);
        new_ptr = (mem_block_header_t *)((char *)new_ptr + (free_head->block_metadata >> 1));
        if (((uintptr_t)free_head->next > (uintptr_t)free_head) && ((uintptr_t)free_head->next < (uintptr_t)(new_ptr)))
        {
            return HEAP_FAILURE;
        }

        if ((uintptr_t)free_head % 16 != 0)
        {
            return HEAP_FAILURE;
        }
        free_head = free_head->next;
    }

    if (((uintptr_t)free_head % 16) != 0)
    {
        return HEAP_FAILURE;
    }

    return HEAP_SUCCESS;
}
