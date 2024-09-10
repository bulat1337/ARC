#include "arc_cache.h"

#include <iostream>
#include <functional>

int main()
{
    const size_t cache_size = 4;

    // Define a cache instance with int as the type
    cache_t<int> cache(cache_size);

    // Function to simulate slow page retrieval
    auto slow_get_page = [](int id) {
        return id; // In this simple case, the page is just the ID itself
    };

    // Initial state
    std::cout << "Initial state:\n";

    // Accessing page 1
    std::cout << "Accessing page 1 (should load into LRU cache)\n";
    cache.lookup_update(1, slow_get_page);
    std::cout << "Expected:\nLRU_cache_: 1\nLFU_cache_: \nLRU_ghost_: \nLFU_ghost_: \n\n";

    // Accessing page 2
    std::cout << "Accessing page 2 (should load into LRU cache)\n";
    cache.lookup_update(2, slow_get_page);
    std::cout << "Expected:\nLRU_cache_: 2 1\nLFU_cache_: \nLRU_ghost_: \nLFU_ghost_: \n\n";

    // Accessing page 3
    std::cout << "Accessing page 3 (should load into LRU cache)\n";
    cache.lookup_update(3, slow_get_page);
    std::cout << "Expected:\nLRU_cache_: 3 2 1\nLFU_cache_: \nLRU_ghost_: \nLFU_ghost_: \n\n";

    // Accessing page 4
    std::cout << "Accessing page 4 (should load into LRU cache)\n";
    cache.lookup_update(4, slow_get_page);
    std::cout << "Expected:\nLRU_cache_: 4 3 2 1\nLFU_cache_: \nLRU_ghost_: \nLFU_ghost_: \n\n";

    // Accessing page 5 (LRU cache should evict page 1)
    std::cout << "Accessing page 5 (should evict page 1 from LRU cache)\n";
    cache.lookup_update(5, slow_get_page);
    std::cout << "Expected:\nLRU_cache_: 5 4 3 2\nLFU_cache_: \nLRU_ghost_: 1\nLFU_ghost_: \n\n";

    // Accessing page 2 (should move page 2 to LFU cache)
    std::cout << "Accessing page 2 (should move page 2 to LFU cache)\n";
    cache.lookup_update(2, slow_get_page);
    std::cout << "Expected:\nLRU_cache_: 2 5 4 3\nLFU_cache_: 2\nLRU_ghost_: 1\nLFU_ghost_: \n\n";

    // Accessing page 6 (LFU cache should evict page 2)
    std::cout << "Accessing page 6 (should evict page 2 from LFU cache)\n";
    cache.lookup_update(6, slow_get_page);
    std::cout << "Expected:\nLRU_cache_: 6 2 5 4\nLFU_cache_: 6\nLRU_ghost_: 1\nLFU_ghost_: 2\n\n";

    // Accessing page 7 (should load into LRU cache and possibly trigger eviction)
    std::cout << "Accessing page 7 (should load into LRU cache and possibly trigger eviction)\n";
    cache.lookup_update(7, slow_get_page);
    std::cout << "Expected:\nLRU_cache_: 7 6 2 5\nLFU_cache_: 6\nLRU_ghost_: 1\nLFU_ghost_: 2\n\n";

    return 0;
}
