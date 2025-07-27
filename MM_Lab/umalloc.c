#include "umalloc.h"
#include "csbrk.h"
#include <stdio.h>
#include <assert.h>
#include "ansicolors.h"

const char author[] = ANSI_BOLD ANSI_COLOR_RED "Vihaan Mehta - vjm655" ANSI_RESET;

/*
 * The following helpers can be used to interact with the mem_block_header_t
 * struct, they can be adjusted as necessary.
 */

mem_block_header_t *free_heads[BIN_COUNT];
const int bucketSizes[4] = {1024, 2848, 4080, 4096};
int binUsage[4] = {0, 0, 0, 0};
int csBrkCt = 0;
int splitCount = 0;
int mallocSuccess = 0;
int coalesceCount = 0;
int nullCount = 0;
int freeCount = 0;
int extensionCheck = 0;

/*
 * select_bin - selects a free list bin to use based on the
 * block size.
 */
mem_block_header_t *select_bin(size_t size)
{

    if (size <= bucketSizes[0] - 16)
    {
        return free_heads[0];
    }
    else if (size > bucketSizes[0] - 16 && size <= bucketSizes[1] - 16)
    {
        return free_heads[1];
    }
    else if (size > bucketSizes[1] - 16 && size <= bucketSizes[2] - 16)
    {
        return free_heads[2];
    }
    else if (size > bucketSizes[2] - 16)
    {
        return free_heads[3];
    }
    else
    {
        printf("ERROR IN SELECT_BIN\n\n");
        return NULL;
    }
}

int find_bucket(size_t size)
{
    if (size <= bucketSizes[0] - 16)
    {
        binUsage[0]++;
        return 0;
    }
    else if (size > bucketSizes[0] - 16 && size <= bucketSizes[1] - 16)
    {
        binUsage[1]++;
        return 1;
    }
    else if (size > bucketSizes[1] - 16 && size <= bucketSizes[2] - 16)
    {
        binUsage[2]++;
        return 2;
    }
    else if (size > bucketSizes[2] - 16)
    {
        binUsage[3]++;
        return 3;
    }
    else
    {
        printf("ERROR IN FIND_BUCKET\n\n");
        return 0;
    }
}

/*
 * block_metadata - returns true if a block is marked as allocated.
 */
bool is_allocated(mem_block_header_t *block)
{
    // Student TODO
    return 0;
}

/*
 * allocate - marks a block as allocated.
 */
void allocate(mem_block_header_t *block)
{
    // Student TODO
}

/*
 * deallocate - marks a block as unallocated.
 */
void deallocate(mem_block_header_t *block)
{
    // Student TODO
}

/*
 * get_size - gets the size of the block.
 */
size_t get_size(mem_block_header_t *block)
{
    // Student TODO
    return sizeof(mem_block_header_t) + get_payload(block);
}

/*
 * get_next - gets the next block.
 */
mem_block_header_t *get_next(mem_block_header_t *block)
{
    // Student TODO
    return NULL;
}

/*
 * set_block_metadata
 * Optional helper method that can be used to initialize the fields for the
 * memory block struct.
 */
void set_block_metadata(mem_block_header_t *block, size_t size, bool alloc)
{

    // Optional student todo
}

/*
 * get_payload - gets the payload of the block.
 */
int get_payload(mem_block_header_t *block)
{
    return block->block_metadata >> 1;
}

/*
 * get_header - given a payload, returns the block.
 */
mem_block_header_t *get_header(void *payload)
{
    // Student TODO
    return NULL;
}

/*
 * The following are helper functions that can be implemented to assist in your
 * design, but they are not required.
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 */
mem_block_header_t *find(size_t payload_size)
{
    // Student TODO
    return NULL;
}

/*
 * extend - extends the heap if more memory is required.
 */
mem_block_header_t *extend(size_t numBytesRequested, int bucketSize)
{

    mem_block_header_t *extension = (mem_block_header_t *) csbrk(numBytesRequested);
    mem_block_header_t *temp = extension;
    
    if (extension == NULL)
    {
        printf("Error occurred in extend \n");
        return NULL;
    }

    int loopCount = numBytesRequested / bucketSize;
    
    for (int x = 0; x < loopCount; x++)
    {
        temp->block_metadata = 0;
        temp->block_metadata |= (bucketSize - sizeof(mem_block_header_t)) << 1;
        
        if (x < loopCount - 1) {
            temp->next = (mem_block_header_t*) ((char*) temp + bucketSize);
            temp = temp->next;
        }
        else {
            temp->next = NULL;
        }
    }
    
    return extension;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 */
mem_block_header_t *split(mem_block_header_t *block, size_t new_block_size)
{
    // Student TODO
    return NULL;
}


void print(int bin) {
    if (free_heads[bin] != NULL) {
        printf("che\n");
        mem_block_header_t* temp1 = free_heads[bin];
        do {
            // address : size : allocation status : next
            printf("      %ld : %ld : %lx : %ld\n", 
                    (uintptr_t) temp1, temp1->block_metadata >> 1, temp1->block_metadata & 0x1,(uintptr_t) temp1->next);
            temp1 = temp1->next;
        } while (temp1);
        printf("End New\n");
    }
    else {
        printf("free_head[bin] was null\n");
    }
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
mem_block_header_t *coalesce(mem_block_header_t *block, mem_block_header_t* next)
{    
        if (block != NULL && next != NULL && (mem_block_header_t *)((char *)block + get_size(block)) == next) {
           coalesceCount++;
           block->block_metadata = 0;
           block->block_metadata |= ((get_payload(block) + get_size(next)) << 1);   
           block->next = next->next;
           
           return block;
           //next->next = NULL;
          // next->block_metadata = 0;
          
           
        }   
    return NULL;
}

/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit()
{
    for (int i = 0; i < BIN_COUNT; i++)
    {
        free_heads[i] = extend(PAGESIZE, 1024);
    }
    
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size)
{
    extensionCheck++;
    size_t sizeWithPadding = size;
    if (sizeWithPadding % 16 != 0) {
        sizeWithPadding += (16 - size % 16);
    }


    int bin = find_bucket(sizeWithPadding);
    mem_block_header_t *head_ptr = free_heads[bin];
    mem_block_header_t *prev_ptr = NULL;
    int fit = 0;
    
    while (head_ptr != NULL)
    {

       int payload = get_payload(head_ptr);

       if ((fit > 2 && payload >= sizeWithPadding))
        {
            
            // First: Set the size and allocation status of newly allocated block. We calculated size with padding before
            head_ptr->block_metadata |= 1;
           
            // Next: We split the memory block if the remaining portion of the free block is more than 64 bytes.
            // Note: need to reorder list
            mem_block_header_t* new_ptr = NULL;
            if (payload - sizeWithPadding >= 64) {
                splitCount++;
            
                // Calculate the address of the new free block
                new_ptr = (mem_block_header_t *)((char *)head_ptr + sizeof(mem_block_header_t) + sizeWithPadding);
            
                // Set the metadata of the new free block
                new_ptr->block_metadata = ((payload - sizeWithPadding - sizeof(mem_block_header_t)) << 1);
            
                // Update the metadata of the allocated block
                head_ptr->block_metadata = (sizeWithPadding << 1) | 1; // Clear previous size and set new size
            
                // Insert the new free block into the free list
                new_ptr->next = head_ptr->next;
            
                if (prev_ptr != NULL) {
                    prev_ptr->next = new_ptr;
                } else {
                    free_heads[bin] = new_ptr;
                }
            }
            else {
            
                
            // Update pointers
            if (prev_ptr != NULL)
            {
                // Update next values
                prev_ptr->next = head_ptr->next;
            }
            else {
                free_heads[bin] = head_ptr->next;
            }
        }

            // Finally, return the pointer
            head_ptr->next = NULL;
            mallocSuccess++;

            return (void *)((char*) head_ptr + sizeof(mem_block_header_t));
        }
        fit++;
        prev_ptr = head_ptr;
        head_ptr = head_ptr->next;    
    }
 
  if (extensionCheck > 25 && free_heads[bin] != NULL) {
        extensionCheck = 0;
        mem_block_header_t* extension = extend(PAGESIZE*2, PAGESIZE);
        mem_block_header_t* traverse = free_heads[bin];
        while (traverse->next != NULL) {
            traverse = traverse->next;
        }
        traverse->next = extension;
    } 


    if (head_ptr == NULL) {
        csBrkCt++;
        
        size_t totalSize = sizeWithPadding + sizeof(mem_block_header_t);
        size_t totalBytesReceived = 0;
        if (totalSize == PAGESIZE) {
             totalBytesReceived = PAGESIZE;
        }
        else {
             totalBytesReceived = (((int) (totalSize / PAGESIZE)) + 1) * PAGESIZE;
        }
        mem_block_header_t *newList = (mem_block_header_t*) csbrk(totalBytesReceived);
        newList->next = NULL;
        newList->block_metadata |= 1;
        newList->block_metadata|= ((sizeWithPadding) << 1) | 1;

        int difference = totalBytesReceived - totalSize;
        if (difference >= 256) {
 
          
                mem_block_header_t *split_ptr = (mem_block_header_t*) ((char*) newList + totalSize);
                split_ptr->next = NULL;

                if (split_ptr == NULL)
                {
                    printf("Error in malloc splitting");
                }

                // Mark the newly split block as free, and note its size
                split_ptr->block_metadata &= 0;
                split_ptr->block_metadata |= ((difference - sizeof(mem_block_header_t))<< 1);
              

                if (free_heads[bin] == NULL) {
                   free_heads[bin] = split_ptr;
                }
                else {
                    orderByAddress(free_heads[bin], split_ptr, bin, true);
                }  
        }    
    
        return (void*) ((char*) newList + sizeof(mem_block_header_t));
    }
     
    return NULL;
}

void orderByAddress(mem_block_header_t *head_ptr, mem_block_header_t *newBlock, int bin, bool single)
{
   
    
    if (head_ptr == NULL || newBlock == NULL || bin < 0 || bin > (BIN_COUNT - 1))
    {
        return;
    }
   

    uintptr_t newAdd = (uintptr_t) newBlock;
    mem_block_header_t *traverse = head_ptr;

    
  
    if (single) {
        if ((uintptr_t) traverse > newAdd) {
            newBlock->next = traverse;
            free_heads[bin] = newBlock;
            return;
        }
        while (traverse->next != NULL)
        {

            if ((uintptr_t)traverse->next > newAdd)
            {
             
                newBlock->next = traverse->next;
                traverse->next = newBlock;
                return;
            }
            traverse = traverse->next;
        }
  

       traverse->next = newBlock;
       return;
    }    
}

/**
 * @param ptr the pointer to the memory to be freed,
 * must have been called by a previous malloc call
 * @brief frees the memory space pointed to by ptr.
 */
void ufree(void *ptr)
{
    if (ptr == NULL) {
        return;
    }
  

    mem_block_header_t *to_free = (mem_block_header_t *)((char*) ptr - sizeof(mem_block_header_t));
    to_free->block_metadata &= ~1;
    to_free->next = NULL;
    int size = get_payload(to_free);
    int bin = find_bucket(size);

    
    

    mem_block_header_t *head_ptr = free_heads[bin];

    if (head_ptr == NULL) {
        free_heads[bin] = to_free;
    }
    else {
        orderByAddress(head_ptr, to_free, bin, true);
    }

    if (freeCount == 200) {
    mem_block_header_t* temp = free_heads[bin];
    mem_block_header_t* prev_ptr = NULL;
    int count = 0;
    while (temp != NULL && count < 10) {
        
        if (coalesce(prev_ptr, temp) == NULL) {
            prev_ptr = temp; 
            temp = temp->next;
        }
        else {
            temp = prev_ptr->next;
        }
        count++;
      }
      freeCount = 0; 
    }
      freeCount++; 
}