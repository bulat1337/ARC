#ifndef ARC_H
#define ARC_H

#include <cstdio>

#include <iostream>
#include <list>
#include <unordered_map>
#include <algorithm>

namespace arc
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

	void shift_cache(	std::list<T>& cache, std::unordered_map<T, ListIt>& hash
						, std::list<T>& ghost_cache, std::unordered_map<T, ListIt>& ghost_hash)
	{
		T removed = cache.back();
		cache.pop_back();
		hash.erase(removed);

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
		HashIt hit = LFU_ghost_hash_.find(id);
		bool in_LFU_ghost = hit != LFU_ghost_hash_.end();

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

	void handle_LRU_hit(const T& id, HashIt hit)
	{
		LRU_cache_.erase(hit->second);
		LRU_hash_.erase(id);

		LFU_cache_.push_front(id);
		LFU_hash_.insert({id, LFU_cache_.begin()});

		MSG("HIT!\n\n");
	}

	void handle_LFU_hit(const T& id, HashIt hit)
	{
		LFU_cache_.erase(hit->second);
		LFU_hash_.erase(id);

		LFU_cache_.push_front(id);
		LFU_hash_.insert({id, LFU_cache_.begin()});
	}

	template <typename F>
	void handle_LRU_ghost_hit(const T& id, HashIt hit, F slow_get_page)
	{
		size_t delta = std::max(	static_cast<size_t>(1)
									, LFU_ghost_cache_.size() / LRU_ghost_cache_.size());
		coeff_ = std::min(coeff_ + delta, sz_);

		replace(id);

		LRU_ghost_cache_.erase(hit->second);
		LRU_ghost_hash_.erase(id);

		T page = slow_get_page(id);
		LFU_cache_.push_front(page);
		LFU_hash_.insert({id, LFU_cache_.begin()});
	}

	template <typename F>
	void handle_LFU_ghost_hit(const T& id, HashIt hit, F slow_get_page)
	{
		size_t delta = std::max(static_cast<size_t>(1), LRU_ghost_cache_.size() / LFU_ghost_cache_.size());
		coeff_ = (coeff_ > delta) ? (coeff_ - delta) : 0;

		replace(id);

		LFU_ghost_cache_.erase(hit->second);
		LFU_ghost_hash_.erase(id);

		T page = slow_get_page(id);
		LFU_cache_.push_front(id);
		LFU_hash_.insert({page, LFU_cache_.begin()});
	}

	void handle_full_LRU(const T& id)
	{
		if (LRU_cache_.size() < sz_)
		{
			MSG("LRU_cache_.size() < sz_\n");

			T removed = LRU_ghost_cache_.back();
			LRU_ghost_cache_.pop_back();
			LRU_ghost_hash_.erase(removed);

			replace(id);
		}
		else
		{
			MSG("LRU_cache_.size() >= sz_\n");

			T removed = LRU_cache_.back();
			LRU_cache_.pop_back();
			LRU_hash_.erase(removed);
		}
	}

	void handle_full_cache(const T& id)
	{
		if (	LRU_cache_.size() + LRU_ghost_cache_.size()
					+ LFU_cache_.size() + LFU_ghost_cache_.size() == 2 * sz_)
		{
			MSG("LRU_cache_.size() + LRU_ghost_cache_.size()\n"
				"+ LFU_cache_.size() + LRU_ghost_cache_.size() == 2 * sz_\n");

			T removed = LFU_ghost_cache_.back();
			LFU_ghost_cache_.pop_back();
			LFU_ghost_hash_.erase(removed);
		}

		replace(id);
	}

	template <typename F>
	void handle_cache_miss(const T& id, F slow_get_page)
	{
		if (LRU_cache_.size() + LRU_ghost_cache_.size() == sz_)
		{
			MSG("LRU_cache_.size() + LRU_ghost_cache_.size() == sz_\n");

			handle_full_LRU(id);
		}
		else if (	(LRU_cache_.size() + LRU_ghost_cache_.size() < sz_) &&
					(	LRU_cache_.size() + LRU_ghost_cache_.size()
						+ LFU_cache_.size() + LFU_ghost_cache_.size() >= sz_) )
		{
			MSG("(LRU_cache_.size() + LRU_ghost_cache_.size() < sz_) &&\n"
				"(	LRU_cache_.size() + LRU_ghost_cache_.size()\n"
				"+ LFU_cache_.size() + LFU_ghost_cache_.size() >= sz_)\n");

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
		#ifdef ENABLE_LOGGING
		std::cerr << "Processing " << id << '\n';
		#endif

		if (sz_ == 0) return false;

		if (auto hit = LRU_hash_.find(id); hit != LRU_hash_.end())
		{
			MSG("Found in LRU\n");

			handle_LRU_hit(id, hit);

			MSG("HIT!\n\n");

			return true;
		}

		if (auto hit = LFU_hash_.find(id); hit != LFU_hash_.end())
		{
			MSG("found in LFU\n");

			handle_LFU_hit(id, hit);

			MSG("HIT!\n\n");

			return true;
		}

		if (auto hit = LRU_ghost_hash_.find(id); hit != LRU_ghost_hash_.end())
		{
			MSG("found in LRU_ghost\n");

			handle_LRU_ghost_hit(id, hit, slow_get_page);

			MSG("MISS!\n\n");

			return false;
		}

		if (auto hit = LFU_ghost_hash_.find(id); hit != LFU_ghost_hash_.end())
		{
			MSG("found in LFU_ghost\n");

			handle_LFU_ghost_hit(id, hit, slow_get_page);

			MSG("MISS!\n\n");

			return false;
		}


		MSG("New element.\n");

		handle_cache_miss(id, slow_get_page);

		MSG("MISS!\n\n");

		return false;
	}
};

}

#endif // ARC_H
