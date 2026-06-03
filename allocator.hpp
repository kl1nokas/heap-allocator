#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

/**
 * @file allocator.hpp
 * @brief Header file for custom heap allocator implementation
 * 
 * This allocator implements a first-fit memory allocation strategy
 * with block splitting and coalescing to manage memory efficiently.
 */

#include <cstddef>  // for size_t

/**
 * @brief Initialize the heap allocator
 * 
 * Sets up the initial free block that spans the entire heap.
 * This function is called automatically on first allocation.
 */
void init_heap();

/**
 * @brief Allocate memory from the heap
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or nullptr on failure
 * 
 * Uses first-fit strategy to find a suitable free block.
 * Allocates memory aligned to alignof(std::max_align_t).
 */
void* alloc(size_t size);

/**
 * @brief Free previously allocated memory
 * @param ptr Pointer to memory to free (can be nullptr)
 * 
 * Marks the block as free and attempts to merge with adjacent free blocks
 * to reduce fragmentation.
 */
void dealloc(void* ptr);

/**
 * @brief Print the current heap structure for debugging
 * 
 * Displays all blocks (free and occupied) with their sizes and positions.
 * Useful for visualizing heap layout and fragmentation.
 */
void print_heap_structure();

/**
 * @brief Get total heap size in bytes
 * @return Total heap size (default: 1MB)
 */
size_t get_heap_size();

/**
 * @brief Get total free space in the heap
 * @return Sum of sizes of all free blocks
 */
size_t get_free_space();

#endif // ALLOCATOR_HPP