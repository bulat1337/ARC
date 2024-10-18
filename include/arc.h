#ifndef ARC_H
#define ARC_H

#include <cstdio>

#include <iostream>
#include <list>
#include <unordered_map>
#include <algorithm>

#include "log.h"

namespace arc
{

template <typename T>
class arc_t
{
  private:
	const size_t sz_ = 0;

	std::list<T> LRU_cache_;
	std::list<T> LFU_cache_;
	std::list<T> LRU_ghost_cache_;
	std::list<T> LFU_ghost_cache_;

	using ListIt = typename std::list<T>::iterator;
	using HashIt = typename std::unordered_map<T, ListIt>::iterator;

	std::unordered_map<T, ListIt> LRU_hash_;
	std::unordered_map<T, ListIt> LFU_hash_;
	std::unordered_map<T, ListIt> LRU_ghost_hash_;
	std::unordered_map<T, ListIt> LFU_ghost_hash_;

	size_t coeff_ = 0;

	void dump_cache() const
	{
		std::clog << "coeff_: " << coeff_ << '\n';
		std::clog << "LRU_cache_: ";
		std::copy(	std::begin(LRU_cache_), std::end(LRU_cache_)
					, std::ostream_iterator<T>(std::clog, " "));
		std::clog << '\n';

		std::clog << "LFU_cache_: ";
		std::copy(	std::begin(LFU_cache_), std::end(LFU_cache_)
					, std::ostream_iterator<T>(std::clog, " "));
		std::clog << '\n';

		std::clog << "LRU_ghost_cache_: ";
		std::copy(	std::begin(LRU_ghost_cache_), std::end(LRU_ghost_cache_)
					, std::ostream_iterator<T>(std::clog, " "));
		std::clog << '\n';

		std::clog << "LFU_ghost_cache_: ";
		std::copy(	std::begin(LFU_ghost_cache_), std::end(LFU_ghost_cache_)
					, std::ostream_iterator<T>(std::clog, " "));
		std::clog << "\n";
	}

	static T remove_oldest(	  std::list<T>& cache
								, std::unordered_map<T, ListIt>& hash)
	{
		T removed = cache.back();
		cache.pop_back();
		hash.erase(removed);

		return removed;
	}

	static void shift_cache(	  std::list<T>& cache
								, std::unordered_map<T, ListIt>& hash
								, std::list<T>& ghost_cache
								, std::unordered_map<T, ListIt>& ghost_hash)
	{
		T removed = remove_oldest(cache, hash);

		ghost_cache.emplace_front(removed);
		ghost_hash.emplace(std::make_pair(removed, ghost_cache.begin()));
	}

	void shift_LRU()
	{
		shift_cache(LRU_cache_, LRU_hash_, LRU_ghost_cache_, LRU_ghost_hash_);
	}

	void shift_LFU()
	{
		shift_cache(LFU_cache_, LFU_hash_, LFU_ghost_cache_, LFU_ghost_hash_);
	}

	void replace(const T& id)
	{
		bool in_LFU_ghost = LFU_ghost_hash_.contains(id);

		if (	!LRU_cache_.empty() &&
					( 	(in_LFU_ghost && LRU_cache_.size() == coeff_) ||
						(LRU_cache_.size() > coeff_) ) )
		{
			shift_LRU();
		}
		else
		{
			shift_LFU();
		}
	}

	void handle_hit(  const T& id, HashIt hit
					, std::list<T>& cache
					, std::unordered_map<T, ListIt>& hash)
	{
		cache.erase(hit->second);
		hash.erase(id);

		LFU_cache_.push_front(id);
		LFU_hash_.insert({id, LFU_cache_.begin()});
	}

	template <typename F>
	void handle_ghost_hit(	  const T& id, HashIt hit, F slow_get_page
								, std::list<T>& ghost_cache
								, std::unordered_map<T, ListIt>& ghost_hash
								, bool inc_coeff)
	{
		if (inc_coeff)
		{
			size_t delta = std::max(1ul, LFU_ghost_cache_.size() / LRU_ghost_cache_.size());
			coeff_ = std::min(coeff_ + delta, sz_);
		}
		else
		{
			size_t delta = std::max(1ul, LRU_ghost_cache_.size() / LFU_ghost_cache_.size());
			coeff_ = (coeff_ > delta) ? (coeff_ - delta) : 0;
		}

		replace(id);

		ghost_cache.erase(hit->second);
		ghost_hash.erase(id);

		T page = slow_get_page(id);
		LFU_cache_.push_front(id);
		LFU_hash_.insert({page, LFU_cache_.begin()});
	}

	void handle_full_LRU(const T& id)
	{
		if (LRU_cache_.size() < sz_)
		{
			MSG("LRU is not full yet.\n");

			remove_oldest(LRU_ghost_cache_, LRU_ghost_hash_);

			replace(id);
		}
		else
		{
			MSG("LRU is full.\n");

			remove_oldest(LRU_cache_, LRU_hash_);
		}
	}

	void handle_full_cache(const T& id)
	{
		if (	LRU_cache_.size() + LRU_ghost_cache_.size()
					+ LFU_cache_.size() + LFU_ghost_cache_.size() == 2 * sz_)
		{
			MSG("Overall size is double the set size\n");

			remove_oldest(LFU_ghost_cache_, LFU_ghost_hash_);
		}

		replace(id);
	}

	template <typename F>
	void handle_cache_miss(const T& id, F slow_get_page)
	{
		if (LRU_cache_.size() + LRU_ghost_cache_.size() == sz_)
		{
			MSG("Overall LRU is full.\n");

			handle_full_LRU(id);
		}
		else if (	(LRU_cache_.size() + LRU_ghost_cache_.size() < sz_) &&
					(	LRU_cache_.size() + LRU_ghost_cache_.size()
						+ LFU_cache_.size() + LFU_ghost_cache_.size() >= sz_) )
		{
			MSG("LRU is not full but overall size is set size.\n");

			handle_full_cache(id);
		}

		T page = slow_get_page(id);
		LRU_cache_.push_front(page);

		LRU_hash_.insert({id, LRU_cache_.begin()});
	}

  public:
	arc_t(const size_t sz) :
		sz_(sz) {}

	template <typename F>
	bool lookup_update(const T& id, F slow_get_page)
	{
		LOG("Processing {}\n", id);

		if (auto hit = LRU_hash_.find(id); hit != LRU_hash_.end())
		{
			MSG("HIT: found in LRU\n");

			handle_hit(id, hit, LRU_cache_, LRU_hash_);

			return true;
		}

		if (auto hit = LFU_hash_.find(id); hit != LFU_hash_.end())
		{
			MSG("HIT: found in LFU\n");

			handle_hit(id, hit, LFU_cache_, LFU_hash_);

			return true;
		}

		if (auto hit = LRU_ghost_hash_.find(id); hit != LRU_ghost_hash_.end())
		{
			MSG("MISS: found in LRU_ghost\n");

			handle_ghost_hit(id, hit, slow_get_page, LRU_ghost_cache_, LRU_ghost_hash_, true);

			return false;
		}

		if (auto hit = LFU_ghost_hash_.find(id); hit != LFU_ghost_hash_.end())
		{
			MSG("MISS: found in LFU_ghost\n");

			handle_ghost_hit(id, hit, slow_get_page, LFU_ghost_cache_, LFU_ghost_hash_, false);

			return false;
		}


		MSG("MISS: New element.\n");

		handle_cache_miss(id, slow_get_page);

		return false;
	}
};

}

#endif // ARC_H
