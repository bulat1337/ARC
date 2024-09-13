#ifndef ARC_CACHE_H
#define ARC_CACHE_H

#include <iostream>
#include <list>
#include <map>

template <typename T>
class arc_cache_t
{
  private:
	size_t sz_ = 0;

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

	size_t coeff = 0;

	#ifdef ENABLE_LOGGING
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
		std::cout << "\n";
	}
	#endif

	void replace(T id)
	{
		HashIt hit = LFU_ghost_hash_.find(id);
		bool in_LFU_ghost = hit != LFU_ghost_hash_.end();

		if (	!LRU_cache_.empty() &&
					( 	(in_LFU_ghost && LRU_cache_.size() == coeff) ||
						(LRU_cache_.size() > coeff) ) )
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
	}

  public:
	arc_cache_t(size_t sz) :
		sz_(sz) {}

	template <typename F>
	bool lookup_update(T id, F slow_get_page)
	{
		#ifdef ENABLE_LOGGING
		std::cout << "Processing " << id << '\n';
		#endif

		if (sz_ == 0) return false;

		HashIt hit;

		if ((hit = LRU_hash_.find(id)) != LRU_hash_.end())
		{
			#ifdef ENABLE_LOGGING
			std::cout << "Found in LRU\n";
			#endif
			// del from T1
			LRU_cache_.erase(hit->second);
			LRU_hash_.erase(id);

			// add in T2
			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			#ifdef ENABLE_LOGGING
			dump_cache();
			std::cout << "HIT!\n\n";
			#endif

			return true;
		}
		else if ((hit = LFU_hash_.find(id)) != LFU_hash_.end())
		{
			#ifdef ENABLE_LOGGING
			std::cout << "found in LFU\n";
			#endif

			// del from T2
			LFU_cache_.erase(hit->second);
			LFU_hash_.erase(id);

			// add in T2
			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			#ifdef ENABLE_LOGGING
			dump_cache();
			std::cout << "HIT!\n\n";
			#endif

			return true;
		}
		else if ((hit = LRU_ghost_hash_.find(id)) != LRU_ghost_hash_.end())
		{
			#ifdef ENABLE_LOGGING
			std::cout << "found in LRU_ghost\n";
			#endif

			size_t delta = std::max(static_cast<size_t>(1), LFU_ghost_.size() / LRU_ghost_.size());
			coeff = std::min(coeff + delta, sz_);

			replace(id);

			// del from B1
			LRU_ghost_.erase(hit->second);
			LRU_ghost_hash_.erase(id);

			// add in T2
			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			#ifdef ENABLE_LOGGING
			dump_cache();
			std::cout << "MISS!\n\n";
			#endif

			return false;
		}
		else if ((hit = LFU_ghost_hash_.find(id)) != LFU_ghost_hash_.end())
		{
			#ifdef ENABLE_LOGGING
			std::cout << "found in LFU_ghost\n";
			#endif

			size_t delta = std::max(static_cast<size_t>(1), LRU_ghost_.size() / LFU_ghost_.size());
			coeff = (coeff > delta) ? (coeff - delta) : 0;

			replace(id);

			// del from B2
			LFU_ghost_.erase(hit->second);
			LFU_ghost_hash_.erase(id);

			// add in T2
			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			#ifdef ENABLE_LOGGING
			dump_cache();
			std::cout << "MISS!\n\n";
			#endif

			return false;
		}

		#ifdef ENABLE_LOGGING
		std::cout << "New element.\n";
		#endif

		T page = slow_get_page(id);

		if (LRU_cache_.size() + LRU_ghost_.size() == sz_)
		{
			#ifdef ENABLE_LOGGING
			std::cout << "LRU_cache_.size() + LRU_ghost_.size() == sz_\n";
			#endif

			if (LRU_cache_.size() < sz_)
			{
				#ifdef ENABLE_LOGGING
				std::cout << "LRU_cache_.size() < sz_\n";
				#endif

				// del from B1
				T removed = LRU_ghost_.back();
				LRU_ghost_.pop_back();
				LRU_ghost_hash_.erase(removed);

				replace(page);
			}
			else
			{
				#ifdef ENABLE_LOGGING
				std::cout << "LRU_cache_.size() >= sz_\n";
				#endif

				// del from T1
				T removed = LRU_cache_.back();
				LRU_cache_.pop_back();
				LRU_hash_.erase(removed);

				// // add to B1
				// LRU_ghost_.push_front(removed);
				// LRU_ghost_hash_.insert({removed, LRU_ghost_.begin()});
			}
		}
		else if (	(LRU_cache_.size() + LRU_ghost_.size() < sz_) &&
					(	LRU_cache_.size() + LRU_ghost_.size()
						+ LFU_cache_.size() + LFU_ghost_.size() >= sz_) )
		{
			#ifdef ENABLE_LOGGING
			std::cout << 	"(LRU_cache_.size() + LRU_ghost_.size() < sz_) &&\n"
							"(	LRU_cache_.size() + LRU_ghost_.size()\n"
								"+ LFU_cache_.size() + LFU_ghost_.size() >= sz_)\n";
			#endif

			if (	LRU_cache_.size() + LRU_ghost_.size()
					+ LFU_cache_.size() + LFU_ghost_.size() == 2 * sz_)
			{
				#ifdef ENABLE_LOGGING
				std::cout << 	"LRU_cache_.size() + LRU_ghost_.size()\n"
								"+ LFU_cache_.size() + LRU_ghost_.size() == 2 * sz_\n";
				#endif

				//del from B2
				T removed = LFU_ghost_.back();
				LFU_ghost_.pop_back();
				LFU_ghost_hash_.erase(removed);
			}

			replace(page);
		}

		// add in T1
		LRU_cache_.push_front(page);
		LRU_hash_.insert({page, LRU_cache_.begin()});

		#ifdef ENABLE_LOGGING
		dump_cache();
		std::cout << "MISS!\n\n";
		#endif

		return false;
	}
};

#endif // ARC_CACHE_H
