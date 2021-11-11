#include "storage/default/large_block_pool.h"
#include "common/lang/string.h"
#include <ghc/filesystem.hpp>
#include <gtest/gtest.h>
namespace fs = ghc::filesystem;

class LBPTest : public ::testing::Test {
public:
  void SetUp() override {
    data_dir = fs::temp_directory_path() /
               ("miniob-sql-test-" + common::random_string(6));
    fs::create_directory(data_dir);
  }

  void TearDown() override { fs::remove_all(data_dir); }
  fs::path data_dir;
};

TEST_F(LBPTest, Create) {
  LargeBlockPool lbp;
  ASSERT_EQ(lbp.open_file(data_dir / "data"), RC::SUCCESS);
}

TEST_F(LBPTest, FirstFreeBlockShouldBeOne) {
  LargeBlockPool lbp;
  ASSERT_EQ(lbp.open_file(data_dir / "data"), RC::SUCCESS);
  ASSERT_EQ(lbp.find_next_free(), 1);
}

TEST_F(LBPTest, FirstSetGetShouldWork) {
  LargeBlockPool lbp;
  ASSERT_EQ(lbp.open_file(data_dir / "data"), RC::SUCCESS);

  auto idx = lbp.find_next_free();
  ASSERT_EQ(lbp.set(idx, "test", 5), RC::SUCCESS);

  auto data = lbp.get(idx);
  ASSERT_TRUE(data != nullptr);
  ASSERT_EQ(std::string(data->data), "test");
}

TEST_F(LBPTest, FirstRemoveShouldWork) {
  LargeBlockPool lbp;
  ASSERT_EQ(lbp.open_file(data_dir / "data"), RC::SUCCESS);

  auto idx = lbp.find_next_free();
  ASSERT_EQ(lbp.set(idx, "test", 5), RC::SUCCESS);

  ASSERT_EQ(lbp.find_next_free(), 2);
  lbp.mark(idx, false);
  ASSERT_EQ(lbp.find_next_free(), 1);
}

TEST_F(LBPTest, LongDataShouldBeCutAndEndWithNull) {
  LargeBlockPool lbp;
  ASSERT_EQ(lbp.open_file(data_dir / "data"), RC::SUCCESS);

  char data[4100];
  strcpy(data + 4090, "123456789");

  auto idx = lbp.find_next_free();
  ASSERT_EQ(lbp.set(idx, data, 4100, true), RC::SUCCESS);

  auto res = lbp.get(idx);
  ASSERT_EQ(std::string(res->data + 4090), "12345");
}

int main(int argc, char **argv) {
  srand((unsigned)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}