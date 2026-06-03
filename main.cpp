/**
 * @file main.cpp
 * @brief Demo application for the custom heap allocator
 *
 * This file demonstrates the usage of the heap allocator
 * with various allocation and deallocation patterns.
 */

#include "allocator.hpp"
#include <iostream>

 /**
  * @brief Main function - demonstrates allocator features
  *
  * Shows:
  * 1. Basic allocation
  * 2. Heap structure visualization
  * 3. Deallocation and reuse
  * 4. Block merging
  */
int main() {
    std::cout << "╔══════════════════════════════════════════════════╗\n";
    std::cout << "║     Custom Heap Allocator Demonstration         ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n\n";

    std::cout << "Total heap size: " << get_heap_size() << " bytes\n";
    std::cout << "Initial free space: " << get_free_space() << " bytes\n\n";

    // ========================================================================
    // Test 1: Basic allocation
    // ========================================================================
    std::cout << "=== Test 1: Basic Allocation ===\n";
    std::cout << "Allocating 100, 200, and 300 bytes...\n";

    void* ptr1 = alloc(100);
    void* ptr2 = alloc(200);
    void* ptr3 = alloc(300);

    if (ptr1 && ptr2 && ptr3) {
        std::cout << "✓ All allocations successful\n";
    }

    print_heap_structure();
    std::cout << "Free space: " << get_free_space() << " bytes\n\n";

    // ========================================================================
    // Test 2: Deallocation
    // ========================================================================
    std::cout << "=== Test 2: Deallocation ===\n";
    std::cout << "Freeing middle block (200 bytes)...\n";
    dealloc(ptr2);

    print_heap_structure();
    std::cout << "Free space: " << get_free_space() << " bytes\n\n";

    // ========================================================================
    // Test 3: Memory reuse
    // ========================================================================
    std::cout << "=== Test 3: Memory Reuse ===\n";
    std::cout << "Allocating 150 bytes (should reuse freed space)...\n";
    void* ptr4 = alloc(150);

    print_heap_structure();
    std::cout << "Free space: " << get_free_space() << " bytes\n\n";

    // ========================================================================
    // Test 4: Block merging
    // ========================================================================
    std::cout << "=== Test 4: Block Merging ===\n";
    std::cout << "Freeing adjacent blocks...\n";
    dealloc(ptr1);
    dealloc(ptr4);

    print_heap_structure();
    std::cout << "Free space: " << get_free_space() << " bytes\n\n";

    // ========================================================================
    // Test 5: Cleanup
    // ========================================================================
    std::cout << "=== Test 5: Cleanup ===\n";
    std::cout << "Freeing remaining block...\n";
    dealloc(ptr3);

    print_heap_structure();
    std::cout << "Free space: " << get_free_space() << " bytes\n";

    std::cout << "\n╔══════════════════════════════════════════════════╗\n";
    std::cout << "║     All tests completed successfully!           ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n";

    return 0;
}