#include <gtest/gtest.h>

extern "C" {
  unsigned calculate_sum(const char *filepath);
}

// Note: must run this program in the same folder that contains "img" folder!

TEST(CalculateSumTest, NonExistFile)
{
  unsigned sum = 0;
  EXPECT_NO_FATAL_FAILURE({ sum = calculate_sum("non-exist-file"); });

  EXPECT_EQ(~(unsigned)0x0, sum);
}

TEST(CalculateSumTest, Logo)
{
  unsigned sum = calculate_sum("img/logo.bin");

  EXPECT_EQ((unsigned)0x02d6e886, sum);
}

TEST(CalculateSumTest, MBR)
{
  unsigned sum = calculate_sum("img/MBR");

  EXPECT_EQ((unsigned)0x000008ea, sum);
}

TEST(CalculateSumTest, EBR1)
{
  unsigned sum = calculate_sum("img/EBR1");

  EXPECT_EQ((unsigned)0x00000a71, sum);
}

TEST(CalculateSumTest, BootImg)
{
  unsigned sum = calculate_sum("img/boot.img");

  EXPECT_EQ((unsigned)0x256b7ff2, sum);
}

TEST(CalculateSumTest, CacheImg)
{
  unsigned sum = calculate_sum("img/cache.img");

  EXPECT_EQ((unsigned)0x0004681f, sum);
}
