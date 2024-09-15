#ifndef PERFECT_CACHE_H
#define PERFECT_CACHE_H

#include <iostream>
#include <map>
#include <list>
#include <vector>

template <typename T>
class perfect_cache
{
  private:
	using ListIt = std::list<T>::iterator;

	std::list<T> cache_;
	std::unordered_map<T, ListIt> hash_;

	size_t sz_;

	std::vector<T> requests_;
	size_t req_counter_ = 0;

	#ifdef ENABLE_LOGGING
	void dump_cache()
	{
		std::clog << "cache_: ";
		for(const auto& elem : cache_)
		{
			std::clog << elem << ' ' ;
		}
		std::clog << "\n\n";
	}
	#endif

  public:
	perfect_cache(size_t sz, const std::vector<T> requests) :
		sz_(sz)
		, requests_(requests) {}

	template <typename F>
	bool lookup_update(T page, F slow_get_page)
	{
		if(sz_ == 0) return false;

		#ifdef ENABLE_LOGGING
		std::clog << "processing: " << page << '\n';
		#endif

		++req_counter_;
		if(hash_.find(page) != hash_.end())
		{
			#ifdef ENABLE_LOGGING
			std::clog << "Found in cache.\n";
			dump_cache();
			#endif

			return true;
		}

		#ifdef ENABLE_LOGGING
		std::clog << "New page.\n";
		#endif

		if(cache_.size() < sz_)
		{
			#ifdef ENABLE_LOGGING
			std::clog << "There is space in cache\n";
			#endif

			T pulled_page = slow_get_page(page);
			cache_.push_front(pulled_page);
			hash_.insert({pulled_page, cache_.begin()});

			#ifdef ENABLE_LOGGING
			dump_cache();
			#endif

			return false;
		}
		else
		{
			#ifdef ENABLE_LOGGING
			std::clog << "Searching for page to replace in cache\n";
			#endif

			size_t farthest_id = 0;
			ListIt page_to_remove   = cache_.begin();

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

							found = true;
							break;
						}

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

			T pulled_page = slow_get_page(page);
			cache_.push_front(pulled_page);
			hash_.insert({pulled_page, cache_.begin()});

			#ifdef ENABLE_LOGGING
			dump_cache();
			#endif

			return false;
		}
	}
};

#endif
