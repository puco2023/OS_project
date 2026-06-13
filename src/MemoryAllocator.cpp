#include "../h/MemoryAllocator.hpp"
#include "../lib/hw.h"

Block* MemoryAllocator::headBlock = nullptr;

void MemoryAllocator::init() {
    MemoryAllocator::headBlock = (Block*)HEAP_START_ADDR;

    MemoryAllocator::headBlock->prev = nullptr;
    MemoryAllocator::headBlock->next = nullptr;
    MemoryAllocator::headBlock->size =
            (char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR;
}

void* MemoryAllocator::mem_alloc(size_t size) {
    if (size == 0) return nullptr;

    int blockNum = (size + sizeof(BlockHeader) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    size_t newSize = blockNum * MEM_BLOCK_SIZE;

    Block* prev = nullptr;

    for (Block* curr = MemoryAllocator::getHeadBlock(); curr; prev = curr, curr = curr->next) {

        if (curr->size == newSize) {
            if (prev) {
                prev->next = curr->next;
            } else {
                MemoryAllocator::setHeadBlock(curr->next);
            }

            if (curr->next) {
                curr->next->prev = prev;
            }

            BlockHeader* header = (BlockHeader*)curr;
            header->size = newSize;

            return (void*)((char*)curr + sizeof(BlockHeader));
        }

        if (curr->size > newSize) {
            Block* newBlk = (Block*)((char*)curr + newSize);

            newBlk->size = curr->size - newSize;
            newBlk->prev = prev;
            newBlk->next = curr->next;

            if (prev) {
                prev->next = newBlk;
            } else {
                MemoryAllocator::setHeadBlock(newBlk);
            }

            if (curr->next) {
                curr->next->prev = newBlk;
            }

            BlockHeader* header = (BlockHeader*)curr;
            header->size = newSize;

            return (void*)((char*)curr + sizeof(BlockHeader));
        }
    }

    return nullptr;
}

int MemoryAllocator::mem_free(void* ptr) {
    if (ptr == nullptr) return -1;

    BlockHeader* header = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    Block* newSeg = (Block*)header;

    newSeg->size = header->size;
    newSeg->prev = nullptr;
    newSeg->next = nullptr;

    Block* curr = MemoryAllocator::getHeadBlock();
    Block* prev = nullptr;

    while (curr != nullptr && (char*)curr < (char*)newSeg) {
        prev = curr;
        curr = curr->next;
    }

    newSeg->prev = prev;
    newSeg->next = curr;

    if (prev) {
        prev->next = newSeg;
    } else {
        MemoryAllocator::setHeadBlock(newSeg);
    }

    if (curr) {
        curr->prev = newSeg;
    }


    if (prev && (char*)prev + prev->size == (char*)newSeg) {
        prev->size += newSeg->size;
        prev->next = newSeg->next;

        if (newSeg->next) {
            newSeg->next->prev = prev;
        }

        newSeg = prev;
    }


    if (newSeg->next && (char*)newSeg + newSeg->size == (char*)newSeg->next) {
        Block* next = newSeg->next;

        newSeg->size += next->size;
        newSeg->next = next->next;

        if (newSeg->next) {
            newSeg->next->prev = newSeg;
        }
    }

    return 0;
}
size_t MemoryAllocator::mem_get_free_space() {
    size_t total = 0;
    for (Block* curr=headBlock;curr;curr=curr->next) {
        total+=curr->size;
    }
    return total;

}
size_t MemoryAllocator::mem_get_largest_free_block()
{
    size_t maxSize = 0;

    for (Block* curr = headBlock; curr; curr = curr->next)
    {
        if (curr->size > maxSize)
        {
            maxSize = curr->size;
        }
    }

    return maxSize;
}
