#include <stddef.h>         // for size_t

#include <iostream>         // for basic_istream, basic_ostream, cin, operat...
#include <unordered_map>    // for unordered_map
#include <vector>           // for vector

#include "perfect_cache.h"  // for perfect_cache_t
#include "process_utils.h"  // for process_pages, set_requests

int main()
{
	size_t cache_size  = 0;
	size_t page_amount = 0;

	size_t hit_amount = 0;

	std::cin >> cache_size >> page_amount;

	std::vector<double> requests;

	std::unordered_map<double, size_t> requests_hash;

	utils::set_requests(requests, requests_hash, page_amount, std::cin);

	perfect_cache::perfect_cache_t<double> p_cache(cache_size, requests, requests_hash);

	hit_amount = utils::process_pages(p_cache, requests, page_amount);

	std::cout << hit_amount << '\n';


    return 0;
}
