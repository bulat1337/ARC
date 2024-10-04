#include "process_utils.h"

#include <ranges>

namespace
{

int slow_get_page(int id)
{
	return id;
}

};

size_t utils::process_pages(arc::arc_t<int>& cache, size_t page_amount, std::istream& in)
{
	size_t hit_amount = 0;

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		int page = 0;
		in >> page;

		if (static_cast<size_t>(cache.lookup_update(page, slow_get_page)))
			++hit_amount;
	}

	return hit_amount;
}

void utils::set_requests(	  std::vector<int>& requests
							, std::unordered_map<int, size_t>& requests_hash
							, size_t page_amount
							, std::istream& in)
{
	requests.reserve(page_amount);

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		int page = 0;
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

size_t utils::process_pages(  perfect_cache::perfect_cache_t<int>& cache
							, const std::vector<int>& requests
							, size_t page_amount)
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
