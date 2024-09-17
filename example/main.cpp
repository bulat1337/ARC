#include <iostream>

#include "arc.h"
#include "perfect_cache.h"

int slow_get_page(int id)
{
    return id;
}



int main()
{
	size_t cache_size  = 0;
	size_t page_amount = 0;

	size_t hit_amount  = 0;
	// size_t perfect_hit_amount = 0;

	std::cin >> cache_size >> page_amount;

	arc<int> cache(cache_size);

	// std::vector<int> requests;
	// requests.reserve(page_amount);

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		int page = 0;
		std::cin >> page;
		// requests.push_back(page);

		hit_amount += static_cast<size_t>(cache.lookup_update(page, slow_get_page));
	}
//
// 	perfect_cache<int> p_cache(cache_size, requests);
//
// 	for(size_t page_id = 0; page_id < page_amount; ++page_id)
// 	{
// 		std::cout << page_id << '\n';
// 		perfect_hit_amount += static_cast<size_t>(	p_cache.lookup_update(requests[page_id]
// 													, slow_get_page));
// 	}

	std::cout << hit_amount << '\n';
	// std::cout << "perfect_hit_amount: " << perfect_hit_amount << '\n';


    return 0;
}
