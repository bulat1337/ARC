#include <iostream>
#include <vector>
#include <unordered_map>

#include "perfect_cache.h"


int slow_get_page(int id)
{
    return id;
}

int main()
{
	size_t cache_size  = 0;
	size_t page_amount = 0;

	size_t perfect_hit_amount = 0;

	std::cin >> cache_size >> page_amount;

	std::vector<int> requests;

	std::unordered_map<int, size_t> requests_hash;
	requests.reserve(page_amount);

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		int page = 0;
		std::cin >> page;
		requests.push_back(page);

		if (requests_hash.find(page) != requests_hash.end())
		{
			requests_hash[page] += 1;
		}
		else
		{
			requests_hash.insert({page, 1});
		}
	}

	perfect_cache<int> p_cache(cache_size, requests, requests_hash);

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		perfect_hit_amount += static_cast<size_t>(	p_cache.lookup_update(requests[page_id]
													, slow_get_page));
	}

	std::cout << perfect_hit_amount << '\n';


    return 0;
}
