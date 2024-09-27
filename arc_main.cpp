#include <iostream>

#include "arc.h"

namespace
{

int slow_get_page(int id)
{
	return id;
}

size_t process_pages(arc::arc_t<int>& cache, size_t page_amount)
{
	size_t hit_amount = 0;

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		int page = 0;
		std::cin >> page;

		if (static_cast<size_t>(cache.lookup_update(page, slow_get_page)))
			++hit_amount;
	}

	return hit_amount;
}

}

int main()
{
	size_t cache_size  = 0;
	size_t page_amount = 0;

	size_t hit_amount  = 0;

	std::cin >> cache_size >> page_amount;

	arc::arc_t<int> cache(cache_size);

	hit_amount = process_pages(cache, page_amount);

	std::cout << hit_amount << '\n';


	return 0;
}
