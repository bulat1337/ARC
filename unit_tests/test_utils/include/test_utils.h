#ifndef TEST_UTILS_H
#define TEST_UTILS_H

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
#include "process_utils.h"

namespace test_utils
{
	template <typename PageType>
	std::string get_result(const std::string& file_name, utils::Cache_type cache_type)
	{
		std::ifstream test_data(file_name);

		if(!test_data.is_open())
			throw std::runtime_error(std::format("Can't open {}\n", file_name));


		size_t cache_size  = 0;
		size_t page_amount = 0;

		size_t hit_amount  = 0;

		test_data >> cache_size >> page_amount;

		switch (cache_type)
		{
			case utils::Cache_type::arc:
			{
				arc::arc_t<PageType> cache(cache_size);

				hit_amount = utils::process_pages(cache, page_amount, test_data);

				break;
			}
			case utils::Cache_type::perfect:
			{
				std::vector<PageType> requests;

				std::unordered_map<PageType, size_t> requests_hash;

				utils::set_requests(requests, requests_hash, page_amount, test_data);

				perfect_cache::perfect_cache_t<PageType> p_cache(cache_size, requests, requests_hash);

				hit_amount = utils::process_pages(p_cache, requests, page_amount);

				break;
			}
			default:
				throw std::logic_error("Unknown cache type\n");
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

	template <typename PageType>
	void run_test(const std::string& test_name, utils::Cache_type cache_type)
	{
		std::string test_path = std::string(TEST_DATA_DIR) + test_name;
		std::string result = get_result<PageType>(test_path + ".dat", cache_type);
		std::string answer = get_answer(test_path + ".ans");

		EXPECT_EQ(result, answer);
	}

}

#endif
