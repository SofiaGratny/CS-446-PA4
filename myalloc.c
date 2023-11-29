//Author: Sofia Gratny
//Date: November 11, 2023
//Purpose: PA4

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <string.h>
#include <sys/mman.h>
#include <stddef.h>

//structure for a memory block in the list
typedef struct _mblock_t {
    struct _mblock_t *prev;
    struct _mblock_t *next;
    size_t size;
    int status;
    void *payload;
} 
    mblock_t;

// structure for the memory list
typedef struct _mlist_t {
    mblock_t *head;
} 
    mlist_t;
    
//function prototypes
void print_memlist(const mblock_t* headptr);
void* mymalloc(size_t size);
void myfree(void* ptr);
mblock_t* findLastMemlistBlock();
mblock_t* findFreeBlockOfSize(size_t size);
void splitBlockAtSize(mblock_t* block, size_t newSize);
void coallesceBlockPrev(mblock_t* freeBlock);
void coallesceBlockNext(mblock_t* freeBlock);
mblock_t* growHeapBySize(size_t size);

//global memory list
mlist_t mlist = { .head = NULL };

int main(int argc, char *argv[]) {
print_memlist(mlist.head);

    //example usage of custom memory allocation functions
    void *p1 = mymalloc(10);
    void *p2 = mymalloc(100);
    void *p3 = mymalloc(200);
    void *p4 = mymalloc(500);
    myfree(p3); p3 = NULL;
    myfree(p2); p2 = NULL;
    void *p5 = mymalloc(150);
    void *p6 = mymalloc(500);
    myfree(p4); p4 = NULL;
    myfree(p5); p5 = NULL;
    myfree(p6); p6 = NULL;
    myfree(p1); p1 = NULL;

print_memlist(mlist.head);

    return 0;
}
//function to print the current state of memory list
void print_memlist(const mblock_t* head) {
    const mblock_t* p = head;
    size_t i = 0;
    while (p != NULL) {
        printf("[%ld] p: %p\n", i, p);
        printf("[%ld] p->size: %ld\n", i, p->size);
        printf("[%ld] p->status: %s\n", i, p->status > 0 ? "allocated" : "free");
        printf("[%ld] p->prev: %p\n", i, p->prev);
        printf("[%ld] p->next: %p\n", i, p->next);
        printf("__________________________\n");
        i++;
        p = p->next;
    }
        printf("==========================\n");
}
//custom malloc function
void* mymalloc(size_t size) {
    mblock_t* freeBlock = findFreeBlockOfSize(size);
        if (freeBlock == NULL) {
        freeBlock = growHeapBySize(size);
        if (freeBlock == NULL) {
        return NULL;
    }
}
    splitBlockAtSize(freeBlock, size);
    return freeBlock->payload;
}
//custom free function
void myfree(void* ptr) {
        if (ptr == NULL) {
        return;
}
    mblock_t* block = (mblock_t*)((char*)ptr - offsetof(mblock_t, payload));
    coallesceBlockPrev(block);
    coallesceBlockNext(block);
}
//find the last block in the memory list
mblock_t* findLastMemlistBlock() {
    mblock_t* current = mlist.head;
    while (current != NULL && current->next != NULL) {
        current = current->next;
}
    return current;
}
//find a free block of the specified size
mblock_t* findFreeBlockOfSize(size_t size) {
    mblock_t* current = mlist.head;
    while (current != NULL) {
            if (current->status == 0 && current->size >= size) {
            return current;
     }
        current = current->next;
}
    return NULL;  
}
//split a block into two at the specified size
void splitBlockAtSize(mblock_t* block, size_t newSize) {
    size_t remaining_size = block->size - newSize;
        if (remaining_size >= sizeof(mblock_t) + 1) {
        mblock_t* remaining_block = (mblock_t*)((char*)block + newSize + offsetof(mblock_t, payload));
        remaining_block->prev = block;
        remaining_block->next = block->next;
        remaining_block->size = remaining_size - offsetof(mblock_t, payload);
        remaining_block->status = 0;
        remaining_block->payload = &(remaining_block->payload);
        block->next = remaining_block;
        block->size = newSize;
    } else 
    {
        block->size += remaining_size;
 }
}
//coalesce a block with the previous block if possible
void coallesceBlockPrev(mblock_t* freeBlock) {
        if (freeBlock->prev != NULL && freeBlock->prev->status == 0) {
        freeBlock->prev->next = freeBlock->next;
        freeBlock->prev->size += freeBlock->size + offsetof(mblock_t, payload);
        if (freeBlock->next != NULL) {
        freeBlock->next->prev = freeBlock->prev;
     }
}
}
//coalesce a block with the next block if possible
void coallesceBlockNext(mblock_t* freeBlock) {
        if (freeBlock->next != NULL && freeBlock->next->status == 0) {
        freeBlock->size += freeBlock->next->size + offsetof(mblock_t, payload);
        freeBlock->next = freeBlock->next->next;
        if (freeBlock->next != NULL) {
        freeBlock->next->prev = freeBlock;
    }
 }
}
//increase the heap size by a specified size
mblock_t* growHeapBySize(size_t size) {
    mblock_t* new_block = (mblock_t*)sbrk(size + offsetof(mblock_t, payload));
        if (new_block == (void*)-1) {
        return NULL;
    }
    new_block->prev = findLastMemlistBlock();
    new_block->next = NULL;
    new_block->size = size;
    new_block->status = 0;
    new_block->payload = &(new_block->payload);
    if (new_block->prev != NULL) {
        new_block->prev->next = new_block;
    } else 
    {
        mlist.head = new_block;
    }
    return new_block;
}

