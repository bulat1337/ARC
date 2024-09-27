#include "test_utils.h"

#include <fstream>
#include <format>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <ranges>

#include <gtest/gtest.h>

#include "perfect_cache.h"
#include "arc.h"

namespace
{

int slow_get_page(int id)
{
	return id;
}

void set_requests(	  std::vector<int>& requests
					, std::unordered_map<int, size_t>& requests_hash
					, size_t page_amount
					, std::istream& input)
{
	requests.reserve(page_amount);

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		int page = 0;
		input >> page;
		requests.push_back(page);

		if (requests_hash.contains(page))
		{
			requests_hash[page] += 1;
		}
		else
		{
			requests_hash.insert({page, 1});
		}
	}
}

size_t process_pages(	  perfect_cache::perfect_cache_t<int>& cache
						, const std::vector<int>& requests
						, size_t page_amount)
{
	size_t hit_amount = 0;

	for (const size_t page_id : std::ranges::views::iota(0ul, page_amount))
	{
		hit_amount = 	cache.lookup_update(requests[page_id], slow_get_page)
						? hit_amount + 1
						: hit_amount;
	}

	return hit_amount;
}

size_t process_pages(arc::arc_t<int>& cache, size_t page_amount, std::istream& input)
{
	size_t hit_amount = 0;

	for(size_t page_id = 0; page_id < page_amount; ++page_id)
	{
		int page = 0;
		input >> page;

		if (static_cast<size_t>(cache.lookup_update(page, slow_get_page)))
			++hit_amount;
	}

	return hit_amount;
}

std::string get_result(const std::string& file_name, bool is_arc)
{
	std::ifstream test_data(file_name);

	if(!test_data.is_open())
		throw std::runtime_error(std::format("Can't open {}\n", file_name));


	size_t cache_size  = 0;
	size_t page_amount = 0;

	size_t hit_amount  = 0;

	test_data >> cache_size >> page_amount;

	if (is_arc)
	{

		arc::arc_t<int> cache(cache_size);

		hit_amount = process_pages(cache, page_amount, test_data);
	}
	else
	{
		std::vector<int> requests;

		std::unordered_map<int, size_t> requests_hash;

		set_requests(requests, requests_hash, page_amount, test_data);

		perfect_cache::perfect_cache_t<int> p_cache(cache_size, requests, requests_hash);

		hit_amount = process_pages(p_cache, requests, page_amount);
	}

	return std::to_string(hit_amount);
}

std::string get_answer(const std::string& file_name)
{
	std::ifstream answer_file(file_name);

	if(!answer_file.is_open())
		throw std::runtime_error(std::format("Can't open {}\n", file_name));

	std::string answer;
	answer_file >> answer;

	return answer;
}

}

void test_utils::run_test(const std::string& test_name, bool is_arc)
{
	std::string test_folder = is_arc ? "/arc_tests/" : "/perfect_tests/";

	std::string test_path = std::string(TEST_DATA_DIR) + test_folder + test_name;
	std::string result = get_result(test_path + ".dat", is_arc);
	std::string answer = get_answer(test_path + ".ans");

	EXPECT_EQ(result, answer);
}
