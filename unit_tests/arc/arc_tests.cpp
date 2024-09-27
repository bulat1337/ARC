#include <fstream>
#include <format>
#include <string>

#include <gtest/gtest.h>

#include "arc.h"
#include "test_utils.h"

TEST(LFU_pattern, cache_1_data_11)
{
	test_utils::run_test("/LFU_pattern/cache-1_data-11", true);
}

TEST(LRU_pattern, cache_5_data_100)
{
	test_utils::run_test("/LRU_pattern/cache-5_data-100", true);
}

TEST(uniform_distribution, cache_5_data_100)
{
	test_utils::run_test("/uniform_distribution/cache-5_data-100", true);
}

TEST(uniform_distribution, cache_10_data_10000)
{
	test_utils::run_test("/uniform_distribution/cache-10_data-10000", true);
}

TEST(common, basic)
{
	test_utils::run_test("/common/basic", true);
}

#ifdef BD_TESTS

TEST(poisson_distribution, cache_10_data_10000000)
{
	test_utils::run_test("/bd_tests/poisson_distribution/cache-10_data-10000000", true);
}

TEST(uniform_distribution, cahce_100_data_10000000)
{
	test_utils::run_test("/bd_tests/uniform_distribution/cahce-100_data-10000000", true);
}

#endif

