#include <stddef.h>         // for size_t
#include <iostream>         // for basic_istream, basic_ostream, cin, operat...

#include "arc.h"            // for arc_t
#include "process_utils.h"  // for process_pages

int main()
{
	size_t cache_size  = 0;
	size_t page_amount = 0;

	size_t hit_amount  = 0;

	std::cin >> cache_size >> page_amount;

	arc::arc_t<int> cache(cache_size);

	hit_amount = utils::process_pages(cache, page_amount, std::cin);

	std::cout << hit_amount << '\n';


	return 0;
}
