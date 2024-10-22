#ifndef DETAIL_H
#define DETAIL_H

#include <iostream>
#include <list>
#include <unordered_map>
#include <algorithm>

namespace detail
{
	template <typename T>
	class cache_base
	{
	  protected:
		using ListIt = typename std::list<T>::iterator;
		using HashIt = typename std::unordered_map<T, ListIt>::iterator;

	  protected:
	  	std::list<T> cache_;
		std::unordered_map<T, ListIt> hash_;

	  public:
		bool lookup_update(const T& id)
		{
			if (auto hit = cache_base<T>::hash_.find(id);
				hit != cache_base<T>::hash_.end())
			{
				handle_hit(id, hit);

				return true;
			}

			return false;
		}

	  	void add(const T& id)
		{
			cache_.push_front(id);
			hash_.insert({id, cache_.begin()});
		}

		bool empty() const { return cache_.empty(); }

		size_t size() const { return cache_.size(); }

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

		virtual void handle_hit(const T& id, HashIt hit) = 0;

		virtual ~cache_base() = default;
	};

	template <typename T>
	class lfu_t : public cache_base<T>
	{
	  private:
	  	void handle_hit(const T& id, cache_base<T>::HashIt hit) override
		{
			cache_base<T>::cache_.erase(hit->second);
			cache_base<T>::hash_.erase(id);

			this->add(id);
		}
	};

	template <typename T>
	class lru_t : public cache_base<T>
	{
	  private:
	  	lfu_t<T>& LFU_;

	  private:
	  	void handle_hit(const T& id, cache_base<T>::HashIt hit) override
		{
			cache_base<T>::cache_.erase(hit->second);
			cache_base<T>::hash_.erase(id);

			LFU_.add(id);
		}

	  public:
	  	lru_t(lfu_t<T>& LFU): LFU_(LFU) {}
	};
};

#endif // DETAIL_H
