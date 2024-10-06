#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H

#include <iostream>

#include <ranges>

#include "arc.h"
#include "perfect_cache.h"

namespace utils
{

template <typename T>
T slow_get_page(T id)
{
	return id;
}

template <typename PageType>
size_t process_pages(arc::arc_t<PageType>& cache, size_t page_amount, std::istream& in)
{
	size_t hit_amount = 0;

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		PageType page = 0;
		in >> page;

		if (static_cast<size_t>(cache.lookup_update(page, slow_get_page<PageType>)))
			++hit_amount;
	}

	return hit_amount;
}

template <typename PageType>
void set_requests(	  std::vector<PageType>& requests
					, std::unordered_map<PageType, size_t>& requests_hash
					, size_t page_amount
					, std::istream& in)
{
	requests.reserve(page_amount);

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		PageType page = 0;
		in >> page;
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

template <typename PageType>
size_t process_pages(  	  perfect_cache::perfect_cache_t<PageType>& cache
						, const std::vector<PageType>& requests
						, size_t page_amount)
{
	size_t hit_amount = 0;

	for (const size_t page_id : std::ranges::views::iota(0ul, page_amount))
	{
		hit_amount = 	cache.lookup_update(requests[page_id], slow_get_page<PageType>)
						? hit_amount + 1
						: hit_amount;
	}

	return hit_amount;
}

}

#endif
