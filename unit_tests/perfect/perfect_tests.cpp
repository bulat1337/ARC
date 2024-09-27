#include <fstream>
#include <format>
#include <string>

#include <gtest/gtest.h>

#include "perfect_cache.h"
#include "test_utils.h"

TEST(edge_cases, ignore)
{
	test_utils::run_test("/edge_cases/ignore", false);
}

TEST(LFU_pattern, cache_1_data_11)
{
	test_utils::run_test("/LFU_pattern/cache-1_data-11", false);
}

TEST(LRU_pattern, cache_5_data_100)
{
	test_utils::run_test("/LRU_pattern/cache-5_data-100", false);
}

TEST(uniform_distribution, cache_5_data_100)
{
	test_utils::run_test("/uniform_distribution/cache-5_data-100", false);
}

TEST(uniform_distribution, cache_10_data_10000)
{
	test_utils::run_test("/uniform_distribution/cache-10_data-10000", false);
}

TEST(common, basic)
{
	test_utils::run_test("/common/basic", false);
}

#ifdef BD_TESTS

TEST(poisson_distribution, cache_10_data_10000000)
{
	test_utils::run_test("/bd_tests/poisson_distribution/cache-10_data-10000000", false);
}

TEST(uniform_distribution, cahce_100_data_10000000)
{
	test_utils::run_test("/bd_tests/uniform_distribution/cahce-100_data-10000000", false);
}

#endif

