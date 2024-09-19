#include <iostream>

#include "arc.h"

int slow_get_page(int id)
{
    return id;
}

int main()
{
	size_t cache_size  = 0;
	size_t page_amount = 0;

	size_t hit_amount  = 0;

	std::cin >> cache_size >> page_amount;

	arc<int> cache(cache_size);

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		int page = 0;
		std::cin >> page;
		// requests.push_back(page);

		hit_amount += static_cast<size_t>(cache.lookup_update(page, slow_get_page));
	}

	std::cout << hit_amount << '\n';


    return 0;
}
