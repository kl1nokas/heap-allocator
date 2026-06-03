# ============================================================================
# CMakeLists.txt - Build configuration for Heap Allocator project
# ============================================================================

# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project definition with name and version
project(HeapAllocator VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
# C++ Standard Configuration
# ============================================================================

# Set C++17 as the required standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Use standard C++ only

# Set output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# Compiler Options
# ============================================================================

# Debug configuration options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)           # Enable debug macros
    add_compile_options(-g -O0 -Wall -Wextra -Wpedantic)  # Debug symbols, no optimization, all warnings
    
# Release configuration options
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)        # Max optimization, disable assertions
    add_compile_options(-Wall)               # Show all warnings
endif()

# Visual Studio specific options
if(MSVC)
    add_compile_options(/W4)                 # Warning level 4
    add_compile_options(/MP)                 # Multi-processor compilation
endif()

# ============================================================================
# Main Executable
# ============================================================================

# Define the main executable and its source files
add_executable(heap_allocator
    src/main.cpp           # Demo application
    src/allocator.cpp      # Allocator implementation
)

# Add include directory for header files
target_include_directories(heap_allocator PRIVATE include)

# Set executable properties
set_target_properties(heap_allocator PROPERTIES
    OUTPUT_NAME "heap_allocator"           # Output binary name
    VERSION ${PROJECT_VERSION}              # Version info
    DEBUG_POSTFIX "_debug"                  # Add _debug suffix for debug builds
)

# ============================================================================
# Unit Tests (Optional)
# ============================================================================

# Option to enable/disable tests (disabled by default)
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    enable_testing()  # Enable CTest
    
    # Test executable
    add_executable(allocator_tests
        tests/test_allocator.cpp   # Test cases
        src/allocator.cpp           # Allocator implementation to test
    )
    
    # Include headers for tests
    target_include_directories(allocator_tests PRIVATE include)
    
    # Define TESTING macro for test-specific code
    target_compile_definitions(allocator_tests PRIVATE TESTING)
    
    # Register test with CTest
    add_test(NAME HeapAllocatorTests COMMAND allocator_tests)
    
    # Optional: Run tests with verbose output
    add_custom_target(check
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS allocator_tests
        COMMENT "Running unit tests..."
    )
endif()

# ============================================================================
# Installation Rules
# ============================================================================

# Install the executable
install(TARGETS heap_allocator
    RUNTIME DESTINATION bin      # On Windows, Linux
    LIBRARY DESTINATION lib      # On Linux/Unix
    ARCHIVE DESTINATION lib      # Static libraries
)

# Install header files for development
install(FILES include/allocator.hpp DESTINATION include)

# ============================================================================
# Custom Targets (Convenience Commands)
# ============================================================================

# Run target: ./heap_allocator
add_custom_target(run
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/heap_allocator
    DEPENDS heap_allocator
    COMMENT "Running heap_allocator demo..."
)

# Clean build directory
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMENT "Cleaning build directory..."
)

# Print build information
add_custom_target(info
    COMMAND ${CMAKE_COMMAND} -E echo "Build Configuration:"
    COMMAND ${CMAKE_COMMAND} -E echo "  Project: ${PROJECT_NAME}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Version: ${PROJECT_VERSION}"
    COMMAND ${CMAKE_COMMAND} -E echo "  C++ Standard: C++${CMAKE_CXX_STANDARD}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Build Type: ${CMAKE_BUILD_TYPE}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Binary Dir: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    COMMENT "Project information"
)