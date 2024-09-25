#ifndef PERFECT_CACHE_H
#define PERFECT_CACHE_H

#include <iostream>
#include <unordered_map>
#include <list>
#include <vector>
#include <algorithm>

namespace perfect_cache
{

#ifdef ENABLE_LOGGING

#define MSG(msg)								\
												\
do												\
{ 												\
	std::fprintf(stderr, msg); 					\
} while (false)									\

#define LOG(msg, ...) 							\
do												\
{ 												\
	std::fprintf(stderr, msg, __VA_ARGS__); 	\
} while (false)									\

#else

#define MSG(msg) do {} while (false)
#define LOG(msg, ...) do {} while (false)

#endif

template <typename T>
class perfect_cache_t
{
  private:
	using ListIt = std::list<T>::iterator;

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
		size_t farthest_id    = 0;
		ListIt page_to_remove = cache_.begin();

		for(ListIt cached = cache_.begin(); cached != cache_.end(); ++cached)
		{
			bool found = false;
			for(size_t request_id = req_counter_; request_id < requests_.size(); ++request_id)
			{
				if(*cached == requests_[request_id])
				{
					if (request_id > farthest_id)
					{
						farthest_id    = request_id;
						page_to_remove = cached;

					}
					found = true;
					break;
				}
			}

			if (!found)
			{
				page_to_remove = cached;
				break;
			}
		}

		#ifdef ENABLE_LOGGING
		std::clog << "page to remove: " << *page_to_remove << '\n';
		std::clog << "farthest_id: " << farthest_id << '\n';
		#endif

		hash_.erase(*page_to_remove);
		cache_.erase(page_to_remove);

		cache_.push_front(pulled_page);
		hash_.emplace(id, cache_.begin());
	}

  public:
	perfect_cache_t(	size_t sz
						, const std::vector<T> requests
						, const std::unordered_map<T, size_t> requests_hash) :
		sz_(sz)
		, requests_(requests)
		, requests_hash_(requests_hash) {}

	template <typename F>
	bool lookup_update(T id, F slow_get_page)
	{
		if(sz_ == 0) return false;

		#ifdef ENABLE_LOGGING
		std::clog << "processing: " << id << '\n';
		#endif

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
			MSG(std::clog << "There is space in cache\n");

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
