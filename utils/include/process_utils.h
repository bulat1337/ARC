#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H

#include <iostream>

#include "arc.h"
#include "perfect_cache.h"

namespace utils
{

enum class Cache_type
{
	  arc
	, perfect
};

size_t process_pages(arc::arc_t<int>& cache, size_t page_amount, std::istream& in);

void set_requests(	  std::vector<int>& requests
					, std::unordered_map<int, size_t>& requests_hash
					, size_t page_amount
					, std::istream& in);

size_t process_pages(     perfect_cache::perfect_cache_t<int>& cache
						, const std::vector<int>& requests
						,  size_t page_amount);

};

#endif
