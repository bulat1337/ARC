#include <iostream>
#include <cassert>
#include "arc_cache.h"

int slow_get_page(int id)
{
    return id;
}

void test_basic_cache_behavior()
{
	arc_cache_t<int> cache(4);

	std::cout << "Expecting miss, adding to LRU\n";
    assert(!cache.lookup_update(1, slow_get_page));

	std::cout << "Expecting miss, adding to LRU\n";
    assert(!cache.lookup_update(2, slow_get_page));

	std::cout << "Expecting miss, adding to LRU\n";
    assert(!cache.lookup_update(3, slow_get_page));

	std::cout << "Expecting miss, adding to LRU\n";
    assert(!cache.lookup_update(4, slow_get_page));

	std::cout << "Expecting miss. Cache overflow, displasing in LRU\n";
    assert(!cache.lookup_update(5, slow_get_page));

    std::cout << "Expecting hit. Moving 3 to LFU\n";
    assert(cache.lookup_update(3, slow_get_page));

    std::cout << "Expecting miss. Moving 1 from LRU_ghost to LFU\n";
    assert(!cache.lookup_update(1, slow_get_page));
}

void test_adaptive_behavior()
{
    arc_cache_t<int> cache(4);

	std::cout << "Expecting miss, adding to LRU\n";
    cache.lookup_update(1, slow_get_page);

	std::cout << "Expecting miss, adding to LRU\n";
    cache.lookup_update(2, slow_get_page);

	std::cout << "Expecting miss, adding to LRU\n";
    cache.lookup_update(3, slow_get_page);

	std::cout << "Expecting miss, adding to LRU\n";
    cache.lookup_update(4, slow_get_page);

    std::cout << "Expecting miss. Displacing in LRU\n";
    cache.lookup_update(5, slow_get_page);

	std::cout << "Expecting miss. Displacing in LRU\n";
    cache.lookup_update(6, slow_get_page);

    std::cout << "Expecting miss. Moving 1 from LRU_ghost to LFU.\n";
    cache.lookup_update(1, slow_get_page);

    std::cout << "Expecting miss. Moving 2 from LRU_ghost to LFU.\n";
    assert(!cache.lookup_update(2, slow_get_page));
}

void test_ghost_list_behavior()
{
    arc_cache_t<int> cache(4);

    std::cout << "Expecting miss, adding to LRU\n";
    cache.lookup_update(1, slow_get_page);

	std::cout << "Expecting miss, adding to LRU\n";
    cache.lookup_update(2, slow_get_page);

	std::cout << "Expecting miss, adding to LRU\n";
    cache.lookup_update(3, slow_get_page);

	std::cout << "Expecting miss, adding to LRU\n";
    cache.lookup_update(4, slow_get_page);

    std::cout << "Expecting miss. Displasing 1 with 5 in LRU.\n";
    cache.lookup_update(5, slow_get_page);

    std::cout << "Expecting miss. Moving 1 from LRU_ghost to LFU.\n";
    cache.lookup_update(1, slow_get_page);

    std::cout << "Expecting miss. Displasing 2 with 6 in LRU.\n";
    cache.lookup_update(6, slow_get_page);

    std::cout << "Expecting miss. Moving 2 from LRU_ghost to LFU.\n";
    assert(!cache.lookup_update(2, slow_get_page));
}

int main()
{
//     std::cout << "Running basic cache behavior test...\n";
//     test_basic_cache_behavior();
//     std::cout << "Basic cache behavior test passed!\n";
//
//     std::cout << "Running adaptive cache behavior test...\n";
//     test_adaptive_behavior();
//     std::cout << "Adaptive cache behavior test passed!\n";
//
//     std::cout << "Running ghost list behavior test...\n";
//     test_ghost_list_behavior();
//     std::cout << "Ghost list behavior test passed!\n";

	size_t cache_size  = 0;
	size_t page_amount = 0;

	size_t hit_amount  = 0;

	std::cin >> cache_size >> page_amount;

	arc_cache_t<int> cache(cache_size);

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		int page = 0;
		std::cin >> page;

		hit_amount += static_cast<size_t>(cache.lookup_update(page, slow_get_page));
	}

	std::cout << hit_amount << '\n';

    return 0;
}
