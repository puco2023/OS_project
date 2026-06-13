#ifndef MEMORYALLOCATOR_HPP
#define MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

struct Block {
    Block* prev;
    Block* next;
    size_t size;
};

struct BlockHeader {
    size_t size;
};

class MemoryAllocator {
private:
    static Block* headBlock;

public:
    static void init();

    static Block* getHeadBlock() {
        return headBlock;
    }

    static void setHeadBlock(Block* b) {
        headBlock = b;
    }

    static void* mem_alloc(size_t size);
    static int mem_free(void* ptr);
    static size_t mem_get_free_space();
    static size_t mem_get_largest_free_block();
};

#endif