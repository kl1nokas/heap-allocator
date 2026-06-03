/**
 * @file test_allocator.cpp
 * @brief Unit tests for the custom heap allocator
 *
 * Comprehensive tests covering allocation, deallocation,
 * alignment, merging, and edge cases.
 */

#include "allocator.hpp"
#include <iostream>
#include <cassert>

 // ============================================================================
 // Test Helpers
 // ============================================================================

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    std::cout << "Testing " << #name << "...\n"; \
    name(); \
    tests_passed++; \
    std::cout << "  ✓ " << #name << " passed\n\n"

#define ASSERT(condition) \
    if (!(condition)) { \
        std::cout << "  ✗ Assertion failed: " << #condition << "\n"; \
        tests_failed++; \
        return; \
    }

// ============================================================================
// Test Cases
// ============================================================================

/**
 * @brief Test basic allocation functionality
 */
void test_basic_allocation() {
    void* p1 = alloc(64);
    void* p2 = alloc(128);
    void* p3 = alloc(256);

    ASSERT(p1 != nullptr);
    ASSERT(p2 != nullptr);
    ASSERT(p3 != nullptr);
    ASSERT(p1 != p2);  // Different allocations should return different pointers
    ASSERT(p2 != p3);

    dealloc(p1);
    dealloc(p2);
    dealloc(p3);
}

/**
 * @brief Test zero-size allocation (should return nullptr)
 */
void test_zero_size() {
    void* p = alloc(0);
    ASSERT(p == nullptr);
}

/**
 * @brief Test null pointer deallocation (should not crash)
 */
void test_null_dealloc() {
    dealloc(nullptr);  // Should do nothing, no crash
    ASSERT(true);
}

/**
 * @brief Test memory alignment
 */
void test_alignment() {
    void* p1 = alloc(1);
    void* p2 = alloc(2);
    void* p3 = alloc(3);

    // Pointers should be aligned to max_align_t (typically 16 bytes)
    uintptr_t addr1 = reinterpret_cast<uintptr_t>(p1);
    uintptr_t addr2 = reinterpret_cast<uintptr_t>(p2);
    uintptr_t addr3 = reinterpret_cast<uintptr_t>(p3);

    ASSERT(addr1 % alignof(std::max_align_t) == 0);
    ASSERT(addr2 % alignof(std::max_align_t) == 0);
    ASSERT(addr3 % alignof(std::max_align_t) == 0);

    dealloc(p1);
    dealloc(p2);
    dealloc(p3);
}

/**
 * @brief Test block merging functionality
 */
void test_block_merging() {
    // Allocate three blocks
    void* p1 = alloc(100);
    void* p2 = alloc(100);
    void* p3 = alloc(100);

    size_t free_before = get_free_space();

    // Free middle and first blocks
    dealloc(p2);
    dealloc(p1);

    // Should merge into a larger free block
    size_t free_after = get_free_space();
    ASSERT(free_after > free_before);

    // Free the last block
    dealloc(p3);

    // Everything should be merged into one big free block
    ASSERT(get_free_space() == get_heap_size());
}

/**
 * @brief Test memory reuse after deallocation
 */
void test_memory_reuse() {
    void* p1 = alloc(100);
    dealloc(p1);

    // Second allocation should reuse the same memory
    void* p2 = alloc(100);

    // With first-fit, it should get the same block
    ASSERT(p1 == p2);

    dealloc(p2);
}

/**
 * @brief Test multiple allocations and deallocations
 */
void test_multiple_ops() {
    const int COUNT = 10;
    void* pointers[COUNT];

    // Allocate multiple blocks
    for (int i = 0; i < COUNT; i++) {
        pointers[i] = alloc(32);
        ASSERT(pointers[i] != nullptr);
    }

    // Free in reverse order
    for (int i = COUNT - 1; i >= 0; i--) {
        dealloc(pointers[i]);
    }

    // All memory should be free
    ASSERT(get_free_space() == get_heap_size());
}

/**
 * @brief Test allocation of maximum size
 */
void test_max_allocation() {
    // Try to allocate almost the entire heap
    size_t max_size = get_heap_size() - sizeof(Block) - 1;
    void* p = alloc(max_size);

    ASSERT(p != nullptr);

    dealloc(p);
}

/**
 * @brief Test allocation failure (request too large)
 */
void test_allocation_failure() {
    // Try to allocate more than heap size
    size_t too_big = get_heap_size() + 1024;
    void* p = alloc(too_big);

    // Should fail and return nullptr
    ASSERT(p == nullptr);
}

/**
 * @brief Test fragmentation handling
 */
void test_fragmentation() {
    // Create fragmentation pattern
    void* p1 = alloc(100);
    void* p2 = alloc(100);
    void* p3 = alloc(100);
    void* p4 = alloc(100);

    // Free alternating blocks
    dealloc(p2);
    dealloc(p4);

    // Should still be able to allocate medium-sized blocks
    void* p5 = alloc(50);
    void* p6 = alloc(50);

    ASSERT(p5 != nullptr);
    ASSERT(p6 != nullptr);

    // Cleanup
    dealloc(p1);
    dealloc(p3);
    dealloc(p5);
    dealloc(p6);
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "\n╔══════════════════════════════════════════════════╗\n";
    std::cout << "║     Heap Allocator Unit Tests                    ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n\n";

    // Initialize heap
    init_heap();

    // Run all tests
    TEST(test_basic_allocation);
    TEST(test_zero_size);
    TEST(test_null_dealloc);
    TEST(test_alignment);
    TEST(test_block_merging);
    TEST(test_memory_reuse);
    TEST(test_multiple_ops);
    TEST(test_max_allocation);
    TEST(test_allocation_failure);
    TEST(test_fragmentation);

    // Print summary
    std::cout << "╔══════════════════════════════════════════════════╗\n";
    std::cout << "║                 Test Summary                     ║\n";
    std::cout << "╠══════════════════════════════════════════════════╣\n";
    std::cout << "║  Tests passed: " << tests_passed << "                              ║\n";
    std::cout << "║  Tests failed: " << tests_failed << "                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n";

    return tests_failed > 0 ? 1 : 0;
}