#ifndef PERFECT_CACHE_H
#define PERFECT_CACHE_H

#include <iostream>
#include <unordered_map>
#include <list>
#include <vector>
#include <algorithm>
#include <format>

#include "log.h"

namespace perfect_cache
{
template <typename T>
class perfect_cache_t
{
  private:
	using ListIt = std::list<T>::iterator;
	using VecIt  = std::vector<T>::iterator;
	using ItDiff = std::vector<T>::iterator::difference_type;

	std::list<T> cache_;
	std::unordered_map<T, ListIt> hash_;

	const size_t sz_;

	std::vector<T> requests_;

	std::unordered_map<T, size_t> requests_hash_;

	size_t req_counter_ = 0;

	void dump_cache() const
	{
		std::clog << "cache_: ";
		std::copy(	std::begin(cache_), std::end(cache_)
					, std::ostream_iterator<T>(std::clog, " "));
		std::clog << "\n\n";
	}

	void replace_farthest_page(const T& id, const T& pulled_page)
	{
		ItDiff farthest_dist    = -1;
		ListIt page_to_remove = cache_.begin();

		for (ListIt cached = cache_.begin(); cached != cache_.end(); ++cached)
		{
			VecIt cached_request = std::find(requests_.begin() + req_counter_, requests_.end(), *cached);

			if (cached_request != requests_.end())
			{
				ItDiff distance = std::distance(requests_.begin(), cached_request);

				if (distance > farthest_dist)
				{
					farthest_dist  = distance;
					page_to_remove = cached;
				}
			}
			else
			{
				page_to_remove = cached;
				break;
			}
		}

		LOG("Page to remove: {}\n", *page_to_remove);
		LOG("farthest_id: {}\n", farthest_id);

		hash_.erase(*page_to_remove);
		cache_.erase(page_to_remove);

		cache_.push_front(pulled_page);
		hash_.emplace(id, cache_.begin());
	}

  public:
	perfect_cache_t(	size_t sz
						, const std::vector<T>& requests
						, const std::unordered_map<T, size_t>& requests_hash) :
		sz_(sz)
		, requests_(requests)
		, requests_hash_(requests_hash) {}

	template <typename F>
	bool lookup_update(T id, F slow_get_page)
	{
		LOG("Processing: {}\n", id);

		++req_counter_;

		if (hash_.find(id) != hash_.end())
		{
			MSG("Found in cache.\n");

			return true;
		}

		MSG("New page.\n");

		if (requests_hash_[id] == 1)
		{
			MSG("The page will not be in the future\n");

			return false;
		}

		T pulled_page = slow_get_page(id);

		if (cache_.size() < sz_)
		{
			MSG("There is space in cache\n");

			cache_.push_front(pulled_page);
			hash_.emplace(pulled_page, cache_.begin());

			return false;
		}

		MSG("Searching for page to replace in cache\n");

		replace_farthest_page(id, pulled_page);

		return false;
	}
};

}

#endif // PERFECT_CACHE
