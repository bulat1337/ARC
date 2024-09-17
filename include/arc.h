#ifndef ARC_H
#define ARC_H

#include <iostream>
#include <list>
#include <unordered_map>

template <typename T>
class arc
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

	size_t coeff_ = 0;

	#ifdef ENABLE_LOGGING
	void dump_cache()
	{
		std::clog << "coeff_: " << coeff_ << '\n';
		std::clog << "LRU_cache_: ";
		for(const auto& elem : LRU_cache_)
		{
			std::clog << elem << ' ';
		}
		std::clog << '\n';

		std::clog << "LFU_cache_: ";
		for(const auto& elem : LFU_cache_)
		{
			std::clog << elem << ' ';
		}
		std::clog << '\n';

		std::clog << "LRU_ghost_: ";
		for(const auto& elem : LRU_ghost_)
		{
			std::clog << elem << ' ';
		}
		std::clog << '\n';

		std::clog << "LFU_ghost_: ";
		for(const auto& elem : LFU_ghost_)
		{
			std::clog << elem << ' ';
		}
		std::clog << "\n";
	}
	#endif

	void shift_LRU()
	{
		T removed = LRU_cache_.back();
		LRU_cache_.pop_back();
		LRU_hash_.erase(removed);

		LRU_ghost_.push_front(removed);
		LRU_ghost_hash_.insert({removed, LRU_ghost_.begin()});
	}

	void shift_LFU()
	{
		T removed = LFU_cache_.back();
		LFU_cache_.pop_back();
		LFU_hash_.erase(removed);

		LFU_ghost_.push_front(removed);
		LFU_ghost_hash_.insert({removed, LFU_ghost_.begin()});
	}

	void replace(T id)
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

  public:
	arc(size_t sz) :
		sz_(sz) {}

	template <typename F>
	bool lookup_update(T id, F slow_get_page)
	{
		#ifdef ENABLE_LOGGING
		std::clog << "Processing " << id << '\n';
		#endif

		if (sz_ == 0) return false;

		HashIt hit;

		if ((hit = LRU_hash_.find(id)) != LRU_hash_.end())
		{
			#ifdef ENABLE_LOGGING
			std::clog << "Found in LRU\n";
			#endif
			LRU_cache_.erase(hit->second);
			LRU_hash_.erase(id);

			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			#ifdef ENABLE_LOGGING
			dump_cache();
			std::clog << "HIT!\n\n";
			#endif

			return true;
		}
		else if ((hit = LFU_hash_.find(id)) != LFU_hash_.end())
		{
			#ifdef ENABLE_LOGGING
			std::clog << "found in LFU\n";
			#endif

			LFU_cache_.erase(hit->second);
			LFU_hash_.erase(id);

			LFU_cache_.push_front(id);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			#ifdef ENABLE_LOGGING
			dump_cache();
			std::clog << "HIT!\n\n";
			#endif

			return true;
		}
		else if ((hit = LRU_ghost_hash_.find(id)) != LRU_ghost_hash_.end())
		{
			#ifdef ENABLE_LOGGING
			std::clog << "found in LRU_ghost\n";
			#endif

			size_t delta = std::max(static_cast<size_t>(1), LFU_ghost_.size() / LRU_ghost_.size());
			coeff_ = std::min(coeff_ + delta, sz_);

			replace(id);

			LRU_ghost_.erase(hit->second);
			LRU_ghost_hash_.erase(id);

			T page = slow_get_page(id);
			LFU_cache_.push_front(page);
			LFU_hash_.insert({id, LFU_cache_.begin()});

			#ifdef ENABLE_LOGGING
			dump_cache();
			std::clog << "MISS!\n\n";
			#endif

			return false;
		}
		else if ((hit = LFU_ghost_hash_.find(id)) != LFU_ghost_hash_.end())
		{
			#ifdef ENABLE_LOGGING
			std::clog << "found in LFU_ghost\n";
			#endif

			size_t delta = std::max(static_cast<size_t>(1), LRU_ghost_.size() / LFU_ghost_.size());
			coeff_ = (coeff_ > delta) ? (coeff_ - delta) : 0;

			replace(id);

			LFU_ghost_.erase(hit->second);
			LFU_ghost_hash_.erase(id);

			T page = slow_get_page(id);
			LFU_cache_.push_front(id);
			LFU_hash_.insert({page, LFU_cache_.begin()});

			#ifdef ENABLE_LOGGING
			dump_cache();
			std::clog << "MISS!\n\n";
			#endif

			return false;
		}

		#ifdef ENABLE_LOGGING
		std::clog << "New element.\n";
		#endif

		if (LRU_cache_.size() + LRU_ghost_.size() == sz_)
		{
			#ifdef ENABLE_LOGGING
			std::clog << "LRU_cache_.size() + LRU_ghost_.size() == sz_\n";
			#endif

			if (LRU_cache_.size() < sz_)
			{
				#ifdef ENABLE_LOGGING
				std::clog << "LRU_cache_.size() < sz_\n";
				#endif

				T removed = LRU_ghost_.back();
				LRU_ghost_.pop_back();
				LRU_ghost_hash_.erase(removed);

				replace(id);
			}
			else
			{
				#ifdef ENABLE_LOGGING
				std::clog << "LRU_cache_.size() >= sz_\n";
				#endif

				T removed = LRU_cache_.back();
				LRU_cache_.pop_back();
				LRU_hash_.erase(removed);
			}
		}
		else if (	(LRU_cache_.size() + LRU_ghost_.size() < sz_) &&
					(	LRU_cache_.size() + LRU_ghost_.size()
						+ LFU_cache_.size() + LFU_ghost_.size() >= sz_) )
		{
			#ifdef ENABLE_LOGGING
			std::clog << 	"(LRU_cache_.size() + LRU_ghost_.size() < sz_) &&\n"
							"(	LRU_cache_.size() + LRU_ghost_.size()\n"
								"+ LFU_cache_.size() + LFU_ghost_.size() >= sz_)\n";
			#endif

			if (	LRU_cache_.size() + LRU_ghost_.size()
					+ LFU_cache_.size() + LFU_ghost_.size() == 2 * sz_)
			{
				#ifdef ENABLE_LOGGING
				std::clog << 	"LRU_cache_.size() + LRU_ghost_.size()\n"
								"+ LFU_cache_.size() + LRU_ghost_.size() == 2 * sz_\n";
				#endif

				T removed = LFU_ghost_.back();
				LFU_ghost_.pop_back();
				LFU_ghost_hash_.erase(removed);
			}

			replace(id);
		}

		T page = slow_get_page(id);
		LRU_cache_.push_front(page);
		
		LRU_hash_.insert({id, LRU_cache_.begin()});

		#ifdef ENABLE_LOGGING
		dump_cache();
		std::clog << "MISS!\n\n";
		#endif

		return false;
	}
};

#endif // ARC_H
