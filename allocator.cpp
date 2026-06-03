/**
 * @file allocator.cpp
 * @brief Implementation of custom heap allocator
 *
 * This file contains the actual implementation of the allocator,
 * including block management, splitting, and coalescing logic.
 */

#include "allocator.hpp"
#include <iostream>
#include <array>
#include <cstddef>

 // ============================================================================
 // Heap Configuration
 // ============================================================================

 // Total heap size: 1 megabyte
constexpr size_t HEAP_SIZE{ 1024 * 1024 };

// Static heap storage - properly aligned for any type
alignas(std::max_align_t) static std::array<std::byte, HEAP_SIZE> heap{};

// ============================================================================
// Block Structure
// ============================================================================

/**
 * @struct Block
 * @brief Memory block header structure
 *
 * Each memory block (free or occupied) has this header at its start.
 * Free blocks form a singly linked list via the 'next' pointer.
 */
struct Block {
    bool free{ true };      // true if block is free, false if occupied
    size_t size{};          // Size of data portion (excluding header)
    Block* next{};          // Pointer to next block (used only in free blocks)
};

// Head of the free list (points to first block in heap)
static Block* head{};

// ============================================================================
// Initialization
// ============================================================================

/**
 * @brief Initialize the heap with a single free block
 *
 * Sets up the initial block that occupies the entire heap.
 */
void init_heap() {
    head = reinterpret_cast<Block*>(heap.data());
    head->free = true;
    head->next = nullptr;
    head->size = HEAP_SIZE;
}

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Align size up to the specified alignment
 * @param size Size to align
 * @param alignment Alignment boundary (must be power of two)
 * @return Aligned size
 */
static size_t align_up(size_t size, size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

/**
 * @brief Split a free block into two blocks
 * @param block Block to split
 * @param size Size to allocate from the block
 *
 * Creates a new free block from the remaining space after allocation.
 * Only splits if remaining space is enough for another block header.
 */
static void split_free_memory_block(Block* block, size_t size) {
    size_t remaining = block->size - size;

    // Don't split if remaining space is too small for a new block header
    if (remaining <= sizeof(Block)) {
        return;
    }

    // Calculate address of the new free block
    auto* new_block{ reinterpret_cast<Block*>(
        reinterpret_cast<std::byte*>(block) + sizeof(Block) + size
    ) };

    // Initialize new block
    new_block->size = remaining - sizeof(Block);
    new_block->free = true;
    new_block->next = block->next;

    // Update current block
    block->size = size;
    block->next = new_block;
}

/**
 * @brief Merge adjacent free blocks to reduce fragmentation
 *
 * Scans the block list and merges consecutive free blocks.
 * Critical for preventing memory fragmentation over time.
 */
static void merge_free_blocks() {
    if (!head) return;

    auto* current{ head };

    while (current && current->next) {
        // If both current and next are free, merge them
        if (current->free && current->next->free) {
            // Absorb the next block into current
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
            // Don't advance - check again with the new next
        }
        else {
            current = current->next;
        }
    }
}

// ============================================================================
// Public API Implementation
// ============================================================================

/**
 * @brief Allocate memory using first-fit strategy
 *
 * Algorithm:
 * 1. Align requested size
 * 2. Initialize heap if needed
 * 3. Search for first free block large enough
 * 4. Split the block if possible
 * 5. Mark as occupied and return data pointer
 */
void* alloc(size_t size) {
    // Edge case: zero-size allocation returns nullptr
    if (size == 0) {
        return nullptr;
    }

    // Lazy initialization - create heap on first allocation
    if (!head) {
        init_heap();
    }

    // Align size to maximum alignment requirement
    size = align_up(size, alignof(std::max_align_t));

    // First-fit search: find first free block that fits
    auto* current{ head };

    while (current) {
        if (current->free && current->size >= size) {
            // Found a suitable block
            split_free_memory_block(current, size);
            current->free = false;
            // Return pointer just after the block header
            return reinterpret_cast<void*>(current + 1);
        }
        current = current->next;
    }

    // No suitable block found - allocation failed
    return nullptr;
}

/**
 * @brief Free allocated memory
 *
 * Algorithm:
 * 1. Validate pointer
 * 2. Get block header (pointer - sizeof(Block))
 * 3. Mark block as free
 * 4. Merge with adjacent free blocks
 */
void dealloc(void* ptr) {
    // Edge case: nullptr is safely ignored
    if (!ptr) {
        return;
    }

    // Get block header by moving back from data pointer
    auto* block{ reinterpret_cast<Block*>(ptr) - 1 };

    // Mark as free
    block->free = true;

    // Merge with neighboring free blocks to reduce fragmentation
    merge_free_blocks();
}

/**
 * @brief Print heap structure for debugging
 *
 * Outputs a list of all blocks with their status (free/occupied)
 * and sizes. Useful for visualizing heap layout.
 */
void print_heap_structure() {
    if (!head) {
        std::cout << "Heap not initialized\n";
        return;
    }

    size_t index{};
    auto* current{ head };

    std::cout << "\n=== Heap Structure ===\n";
    while (current) {
        std::cout << "[block " << ++index << "]: "
            << (current->free ? "FREE     " : "OCCUPIED ")
            << ", " << current->size << " bytes\n";
        current = current->next;
    }
    std::cout << "=====================\n\n";
}

/**
 * @brief Get total heap size
 * @return HEAP_SIZE constant
 */
size_t get_heap_size() {
    return HEAP_SIZE;
}

/**
 * @brief Calculate total free space
 * @return Sum of sizes of all free blocks
 *
 * Scans the block list and accumulates sizes of free blocks.
 */
size_t get_free_space() {
    if (!head) return HEAP_SIZE;

    size_t free_space = 0;
    auto* current = head;

    while (current) {
        if (current->free) {
            free_space += current->size;
        }
        current = current->next;
    }

    return free_space;
}