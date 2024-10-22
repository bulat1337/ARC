#ifndef ARC_H
#define ARC_H

#include <cstdio>

#include <iostream>
#include <list>
#include <unordered_map>
#include <algorithm>

#include "log.h"
#include "detail.h"

namespace arc
{

template <typename T>
class arc_t
{
  private:
	using ListIt = typename std::list<T>::iterator;
	using HashIt = typename std::unordered_map<T, ListIt>::iterator;

  private:
	class ghost_t
	{
	  private:
		arc_t& outer_;

	  	std::list<T> cache_;
		std::unordered_map<T, ListIt> hash_;

		const bool inc_coeff_;

	  public:
		ghost_t(arc_t& outer, bool inc_coeff = false):
			  outer_(outer)
			, inc_coeff_(inc_coeff) {}

	  	template <typename F>
		bool lookup_update(const T& id, F slow_get_page)
		{
			if (auto hit = hash_.find(id); hit != hash_.end())
			{
				MSG("MISS: found in LRU_ghost\n");

				outer_.handle_ghost_hit(id, hit, slow_get_page, cache_, hash_, inc_coeff_);

				return true;
			}

			return false;
		}

		size_t size() const { return cache_.size(); }

		bool contains(const T& id) const { return hash_.contains(id); }

		void emplace_front(const T& page)
		{
			cache_.emplace_front(page);
			hash_.emplace(std::make_pair(page, cache_.begin()));
		}

		T remove_oldest()
		{
			T removed = cache_.back();
			cache_.pop_back();
			hash_.erase(removed);

			return removed;
		}

		void dump() const
		{
			std::copy(	  std::begin(cache_), std::end(cache_)
						, std::ostream_iterator<T>(std::clog, " "));

			std::clog << '\n';
		}
	};

  private:
	const size_t sz_ = 0;

	detail::lfu_t<T> LFU_;
	detail::lru_t<T> LRU_{LFU_};

	ghost_t LRU_ghost_{*this, true};
	ghost_t LFU_ghost_{*this, false};

	size_t coeff_ = 0;

	void dump_cache() const
	{
		std::clog << "coeff_: " << coeff_ << '\n';
		std::clog << "LRU_cache_: ";
		LRU_.dump();

		std::clog << "LFU_cache_: ";
		LFU_.dump();

		std::clog << "LRU_ghost_cache_: ";
		LRU_ghost_.dump();

		std::clog << "LFU_ghost_cache_: ";
		LFU_ghost_.dump();
	}

	static void shift_cache(detail::cache_base<T>& cache, ghost_t& ghost)
	{
		T removed = cache.remove_oldest();

		ghost.emplace_front(removed);
	}

	void shift_LRU()
	{
		shift_cache(LRU_, LRU_ghost_);
	}

	void shift_LFU()
	{
		shift_cache(LFU_, LFU_ghost_);
	}

	void replace(const T& id)
	{
		bool in_LFU_ghost = LFU_ghost_.contains(id);

		if (	!LRU_.empty() &&
				( 	(in_LFU_ghost && LRU_.size() == coeff_) ||
					(LRU_.size() > coeff_) ) )
		{
			shift_LRU();
		}
		else
		{
			shift_LFU();
		}
	}

	template <typename F>
	void handle_ghost_hit(	  const T& id, HashIt hit, F slow_get_page
							, std::list<T>& ghost_cache
							, std::unordered_map<T, ListIt>& ghost_hash
							, bool inc_coeff)
	{
		if (inc_coeff)
		{
			size_t delta = std::max(1ul, LFU_ghost_.size() / LRU_ghost_.size());
			coeff_ = std::min(coeff_ + delta, sz_);
		}
		else
		{
			size_t delta = std::max(1ul, LRU_ghost_.size() / LFU_ghost_.size());
			coeff_ = (coeff_ > delta) ? (coeff_ - delta) : 0;
		}

		replace(id);

		ghost_cache.erase(hit->second);
		ghost_hash.erase(id);

		T page = slow_get_page(id);
		LFU_.add(page);
	}

	void handle_full_LRU(const T& id)
	{
		if (LRU_.size() < sz_)
		{
			MSG("LRU is not full yet.\n");

			LRU_ghost_.remove_oldest();

			replace(id);
		}
		else
		{
			MSG("LRU is full.\n");

			LRU_.remove_oldest();
		}
	}

	void handle_full_cache(const T& id)
	{
		if (LRU_.size() + LRU_ghost_.size() +
			LFU_.size() + LFU_ghost_.size() == 2 * sz_)
		{
			MSG("Overall size is double the set size\n");

			LFU_ghost_.remove_oldest();
		}

		replace(id);
	}

	template <typename F>
	void handle_cache_miss(const T& id, F slow_get_page)
	{
		if (LRU_.size() + LRU_ghost_.size() == sz_)
		{
			MSG("Overall LRU is full.\n");

			handle_full_LRU(id);
		}
		else if (	(LRU_.size() + LRU_ghost_.size() < sz_) &&
					(	LRU_.size() + LRU_ghost_.size() +
						LFU_.size() + LFU_ghost_.size() >= sz_) )
		{
			MSG("LRU is not full but overall size is set size.\n");

			handle_full_cache(id);
		}

		T page = slow_get_page(id);
		LRU_.add(page);
	}

  public:
	arc_t(const size_t sz): sz_(sz) {}

	template <typename F>
	bool lookup_update(const T& id, F slow_get_page)
	{
		LOG("Processing {}\n", id);

		if (LRU_.lookup_update(id)) return true;

		if (LFU_.lookup_update(id)) return true;

		if (LRU_ghost_.lookup_update(id, slow_get_page)) return false;

		if (LFU_ghost_.lookup_update(id, slow_get_page)) return false;

		MSG("MISS: New element.\n");

		handle_cache_miss(id, slow_get_page);

		return false;
	}
};

}

#endif // ARC_H
