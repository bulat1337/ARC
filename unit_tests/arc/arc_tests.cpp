#include <fstream>
#include <format>
#include <string>

#include <gtest/gtest.h>

#include "arc.h"
#include "test_utils.h"

TEST(LFU_pattern, cache_1_data_11)
{
	test_utils::run_test<int>("/LFU_pattern/cache-1_data-11", utils::Cache_type::arc);
}

TEST(LRU_pattern, cache_5_data_100)
{
	test_utils::run_test<int>("/LRU_pattern/cache-5_data-100", utils::Cache_type::arc);
}

TEST(uniform_distribution, cache_5_data_100)
{
	test_utils::run_test<int>("/uniform_distribution/cache-5_data-100", utils::Cache_type::arc);
}

TEST(uniform_distribution, cache_10_data_10000)
{
	test_utils::run_test<int>("/uniform_distribution/cache-10_data-10000", utils::Cache_type::arc);
}

TEST(common, basic)
{
	test_utils::run_test<int>("/common/basic", utils::Cache_type::arc);
}

#ifdef BD_TESTS

TEST(poisson_distribution, cache_10_data_10000000)
{
	test_utils::run_test<int>(	  "/bd_tests/poisson_distribution/cache-10_data-10000000"
							, utils::Cache_type::arc);
}

TEST(uniform_distribution, cahce_100_data_10000000)
{
	test_utils::run_test<int>(	  "/bd_tests/uniform_distribution/cahce-100_data-10000000"
							, utils::Cache_type::arc);
}

#endif

