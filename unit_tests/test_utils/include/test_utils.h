#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <ranges>
#include <filesystem>
#include <string_view>

#include <gtest/gtest.h>

#include "perfect_cache.h"
#include "arc.h"
#include "process_utils.h"

namespace test_utils
{
	enum class Cache_type
	{
		arc
		, perfect
	};

	template <typename PageType>
	size_t get_result(std::string_view file_name, test_utils::Cache_type cache_type)
	{
		std::ifstream test_data;

		test_data.exceptions(std::ifstream::badbit | std::ifstream::failbit);

		test_data.open(std::string(file_name));

		size_t cache_size  = 0;
		size_t page_amount = 0;

		size_t hit_amount  = 0;

		test_data >> cache_size >> page_amount;

		switch (cache_type)
		{
			case test_utils::Cache_type::arc:
			{
				arc::arc_t<PageType> cache(cache_size);

				hit_amount = utils::process_pages(cache, page_amount, test_data);

				break;
			}
			case test_utils::Cache_type::perfect:
			{
				std::vector<PageType> requests;

				std::unordered_map<PageType, size_t> requests_hash;

				utils::set_requests(requests, requests_hash, page_amount, test_data);

				perfect_cache::perfect_cache_t<PageType> p_cache(	  cache_size
																	, requests
																	, requests_hash);

				hit_amount = utils::process_pages(p_cache, requests, page_amount);

				break;
			}
			default:
				throw std::logic_error("Unknown cache type\n");
		}

		return hit_amount;
	}

	inline size_t get_answer(std::string_view file_name)
	{
		std::ifstream answer_file;

		answer_file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

		answer_file.open(std::string(file_name));

		size_t answer;
		answer_file >> answer;

		return answer;
	}

	template <typename PageType>
	void run_test(std::string_view test_name, test_utils::Cache_type cache_type)
	{
		std::filesystem::path test_dir  = TEST_DATA_DIR;

		std::filesystem::path test_path = test_dir / test_name;

		test_path.replace_extension(".dat");

		size_t answer;
		size_t result;

		result = get_result<PageType>(test_path.string(), cache_type);

		test_path.replace_extension(".ans");

		answer = get_answer(test_path.string());

		EXPECT_EQ(result, answer);
	}

}

#endif
