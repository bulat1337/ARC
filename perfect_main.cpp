#include <iostream>
#include <vector>
#include <unordered_map>
#include <ranges>

#include "perfect_cache.h"


namespace
{

int slow_get_page(int id)
{
    return id;
}

void set_requests(	  std::vector<int>& requests
					, std::unordered_map<int, size_t>& requests_hash
					, size_t page_amount)
{
	requests.reserve(page_amount);

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		int page = 0;
		std::cin >> page;
		requests.push_back(page);

		if (requests_hash.contains(page))
		{
			requests_hash[page] += 1;
		}
		else
		{
			requests_hash.insert({page, 1});
		}
	}
}

size_t process_pages(	  perfect_cache::perfect_cache_t<int>& cache
						, const std::vector<int>& requests
						,  size_t page_amount)
{
	size_t hit_amount = 0;

	for (const size_t page_id : std::ranges::views::iota(0ul, page_amount))
	{
		hit_amount = 	cache.lookup_update(requests[page_id], slow_get_page)
						? hit_amount + 1
						: hit_amount;
	}

	return hit_amount;
}

}

int main()
{
	size_t cache_size  = 0;
	size_t page_amount = 0;

	size_t hit_amount = 0;

	std::cin >> cache_size >> page_amount;

	std::vector<int> requests;

	std::unordered_map<int, size_t> requests_hash;

	set_requests(requests, requests_hash, page_amount);

	perfect_cache::perfect_cache_t<int> p_cache(cache_size, requests, requests_hash);

	hit_amount = process_pages(p_cache, requests, page_amount);

	std::cout << hit_amount << '\n';


    return 0;
}
