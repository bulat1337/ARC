#include <fstream>
#include <format>
#include <string>

#include <gtest/gtest.h>

#include "perfect_cache.h"
#include "test_utils.h"

TEST(edge_cases, ignore)
{
	test_utils::run_test<int>("edge_cases/ignore", test_utils::Cache_type::perfect);
}

TEST(LFU_pattern, cache_1_data_11)
{
	test_utils::run_test<int>("LFU_pattern/cache-1_data-11", test_utils::Cache_type::perfect);
}

TEST(LFU_pattern, cache_1_data_11_fp)
{
	test_utils::run_test<double>("LFU_pattern/cache-1_data-11_fp", test_utils::Cache_type::perfect);
}

TEST(LRU_pattern, cache_5_data_100)
{
	test_utils::run_test<int>("LRU_pattern/cache-5_data-100", test_utils::Cache_type::perfect);
}

TEST(uniform_distribution, cache_5_data_100)
{
	test_utils::run_test<int>("uniform_distribution/cache-5_data-100", test_utils::Cache_type::perfect);
}

TEST(uniform_distribution, cache_10_data_10000)
{
	test_utils::run_test<int>("uniform_distribution/cache-10_data-10000", test_utils::Cache_type::perfect);
}

TEST(common, basic)
{
	test_utils::run_test<int>("common/basic", test_utils::Cache_type::perfect);
}

#ifdef BD_TESTS

TEST(poisson_distribution, cache_10_data_10000000)
{
	test_utils::run_test<int>(	  "bd_tests/poisson_distribution/cache-10_data-10000000"
								, test_utils::Cache_type::perfect);
}

TEST(uniform_distribution, cahce_100_data_10000000)
{
	test_utils::run_test<int>(	  "bd_tests/uniform_distribution/cahce-100_data-10000000"
								, test_utils::Cache_type::perfect);
}

#endif

