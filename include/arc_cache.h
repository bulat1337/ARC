#ifndef ARC_CACHE_H
#define ARC_CACHE_H

#include <iostream>
#include <list>
#include <map>

template <typename T>
class cache_t
{
  private:
	size_t sz_;

	std::list<T> LRU_cache_;
	std::list<T> LFU_cache_;
	std::list<T> LRU_ghost_;
	std::list<T> LFU_ghost_;

	using ListIt = typename std::list<T>::iterator;
	using HashIt = typename std::unordered_map<T, ListIt>::iterator;

	std::unordered_map<T, ListIt> LRU_hash_;
	std::unordered_map<T, ListIt> LFU_hash_;
	std::unordered_map<T, ListIt> LRU_ghost_hash_;
	std::unordered_map<T, ListIt> LFU_ghost_hash_;

	size_t coeff;

	void dump_cache()
	{
		std::cout << "coeff: " << coeff << '\n';
		std::cout << "LRU_cache_: ";
		for(const auto& elem : LRU_cache_)
		{
			std::cout << elem << ' ';
		}
		std::cout << '\n';

		std::cout << "LFU_cache_: ";
		for(const auto& elem : LFU_cache_)
		{
			std::cout << elem << ' ';
		}
		std::cout << '\n';

		std::cout << "LRU_ghost_: ";
		for(const auto& elem : LRU_ghost_)
		{
			std::cout << elem << ' ';
		}
		std::cout << '\n';

		std::cout << "LFU_ghost_: ";
		for(const auto& elem : LFU_ghost_)
		{
			std::cout << elem << ' ';
		}
		std::cout << "\n\n";
	}

	bool is_full() const
	{
		return (LRU_hash_.size() + LFU_hash_.size()) > sz_;
	}

	void smart_evict()
	{
		if (LRU_cache_.size() > coeff)
		{
			// add in B1
			LRU_ghost_.push_front(LRU_cache_.back());
			LRU_ghost_hash_.insert({LRU_ghost_.front(), LRU_ghost_.begin()});

			// del from T1
			LRU_hash_.erase(LRU_cache_.back());
			LRU_cache_.pop_back();
		}
		else
		{
			// add in B2
			LFU_ghost_.push_front(LFU_cache_.back());
			LFU_ghost_hash_.insert({LFU_ghost_.front(), LFU_ghost_.begin()});

			// del from T2
			LFU_hash_.erase(LFU_cache_.back());
			LFU_cache_.pop_back();
		}
	}

  public:
	cache_t(size_t sz) :
		sz_(sz)
		, coeff(sz / 2) {}

	template <typename F>
	bool lookup_update(T id, F slow_get_page)
	{
		HashIt hit;

		if ((hit = LRU_hash_.find(id)) != LRU_hash_.end())
		{
			// del from T1
			LRU_cache_.erase(hit->second);
			LRU_hash_.erase(id);

			// add in B1
			LRU_ghost_.push_front(id);
			LRU_ghost_hash_.insert({id, LRU_ghost_.begin()});

			// add in T2
			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			dump_cache();
			return true;
		}
		else if (LFU_hash_.find(id) != LFU_hash_.end())
		{
			dump_cache();
			return true;
		}
		else if ((hit = LRU_ghost_hash_.find(id)) != LRU_ghost_hash_.end())
		{
			++coeff;

			// del from B1
			LRU_ghost_.erase(hit->second);
			LRU_ghost_hash_.erase(id);

			// add in T2
			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			if(is_full())
			{
				// add in B1
				LRU_ghost_.push_front(LRU_cache_.back());
				LRU_ghost_hash_.insert({LRU_ghost_.front(), LRU_ghost_.begin()});

				// del from T1
				LRU_hash_.erase(LRU_cache_.back());
				LRU_cache_.pop_back();
			}
		}
		else if ((hit = LFU_ghost_hash_.find(id)) != LFU_ghost_hash_.end())
		{
			--coeff;

			// del from B2
			LFU_ghost_.erase(hit->second);
			LFU_ghost_hash_.erase(id);

			// add in T2
			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			if(is_full())
			{
				smart_evict();
			}
		}
		else
		{
			T page = slow_get_page(id);
			LRU_cache_.push_front(page);
			LRU_hash_.insert({id, LRU_cache_.begin()});

			if(is_full())
			{
				smart_evict();
			}
		}

		dump_cache();
		return false;
	}
};

#endif // ARC_CACHE_H
