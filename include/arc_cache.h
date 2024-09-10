#ifndef ARC_CACHE_H
#define ARC_CACHE_H

#include <iostream>
#include <list>
#include <map>

template <typename T>
class arc_cache_t
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
		return (LRU_hash_.size() + LFU_hash_.size()) >= sz_;
	}

	void adjust_cache_size()
	{
		if (LRU_cache_.size() > coeff)
		{
			// del from T1
			T removed = LRU_cache_.back();
			LRU_cache_.pop_back();
			LRU_hash_.erase(removed);

			// add to B1
			LRU_ghost_.push_front(removed);
			LRU_ghost_hash_.insert({removed, LRU_ghost_.begin()});
		}
		else
		{
			//del from T2
			T removed = LFU_cache_.back();
			LFU_cache_.pop_back();
			LFU_hash_.erase(removed);

			// add to B2
			LFU_ghost_.push_front(removed);
			LFU_ghost_hash_.insert({removed, LFU_ghost_.begin()});
		}

		if (LRU_ghost_.size() > sz_)
		{
			T removed = LRU_ghost_.back();
			LRU_ghost_.pop_back();
			LRU_ghost_hash_.erase(removed);
		}

		if (LFU_ghost_.size() > sz_)
		{
			T removed = LFU_ghost_.back();
			LFU_ghost_.pop_back();
			LFU_ghost_hash_.erase(removed);
		}
	}

  public:
	arc_cache_t(size_t sz) :
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
			// p = min(p + max(1, |B2| / |B1|), C)
			coeff = std::min(	coeff + std::max(	static_cast<size_t>(1)
													, LFU_ghost_.size() / LRU_ghost_.size())
								, sz_);

			// del from B1
			LRU_ghost_.erase(hit->second);
			LRU_ghost_hash_.erase(id);

			// add in T2
			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			adjust_cache_size();

			dump_cache();

			return false;
		}
		else if ((hit = LFU_ghost_hash_.find(id)) != LFU_ghost_hash_.end())
		{
			// p = max(p - max(1, |B1| / |B2|), 0)
			coeff = std::max(	coeff - std::max(	static_cast<size_t>(1)
													, LRU_ghost_.size() / LFU_ghost_.size())
								, static_cast<size_t>(0));

			// del from B2
			LFU_ghost_.erase(hit->second);
			LFU_ghost_hash_.erase(id);

			// add in T2
			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			adjust_cache_size();

			dump_cache();

			return false;
		}

		if(is_full())
		{
			adjust_cache_size();
		}

		T page = slow_get_page(id);
		LRU_cache_.push_front(page);
		LRU_hash_.insert({id, LRU_cache_.begin()});

		dump_cache();
		return false;
	}
};

#endif // ARC_CACHE_H
