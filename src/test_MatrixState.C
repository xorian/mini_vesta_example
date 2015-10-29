#include <gtest/gtest.h>

#include "matrix_state.hpp"

// ----------------------------------------------------------------------
// Fixed size 16x16 tests, written first
// ----------------------------------------------------------------------
namespace {
  // Test that an empty matrix is, in fact, empty
  TEST(MatrixStateTest, EmptyMatrix) {
    MatrixState<16,16> state;
    EXPECT_TRUE(state.begin() == state.end());
    unsigned int on_count = 0;
    for(auto pos : state) {
      on_count++;
    }
    EXPECT_EQ(on_count, 0);
    for(unsigned int row = 0; row < 16; row++) {
      EXPECT_EQ(state.get_row(row), 0);
    }
    for(unsigned int row = 0; row < 16; row++) {
      for(unsigned int col = 0; col < 16; col++) {
	EXPECT_FALSE(state.get(row,col));
      }
    }
  }
  // Construct a diagonal matrix state and check it
  TEST(MatrixStateTest, DiagMatrix) {
    MatrixState<16,16> state1, state2;
    for(unsigned int i = 0; i < 16; i++) {
      state1.set(i, i);
      state2.set(i, 15-i);
    }
    unsigned int on_count = 0;
    for(auto pos : state1) {
      EXPECT_EQ(pos.first, pos.second);
      on_count++;
    }
    EXPECT_EQ(on_count, 16);
    on_count = 0;
    for(auto pos : state2) {
      EXPECT_EQ(15-pos.first, pos.second);
      on_count++;
    }
    EXPECT_EQ(on_count, 16);
    for(unsigned int row = 0; row < 16; row++) {
      EXPECT_EQ(state1.get_row(row), 1<<row);
      EXPECT_EQ(state2.get_row(row), 1<<(15-row));
    }
    EXPECT_NE(state1, state2);
  }

  // Construct an inverse diagonal matrix (diagonals off, everything
  // else on)
  TEST(MatrixStateTest, InverseDiagMatrix) {
    MatrixState<16,16> state1, state2;
    for(unsigned int i = 0; i < 16; i++) {
      state1.write_row(i, ~(MatrixState<16,16>::row_t)0);
      state1.clear(i, i);
      state2.write_row(i, ~(MatrixState<16,16>::row_t)0);
      state2.clear(i, 15-i);
    }
    unsigned int on_count = 0;
    for(auto pos : state1) {
      EXPECT_NE(pos.first, pos.second);
      on_count++;
    }
    EXPECT_EQ(on_count, 240);
    on_count = 0;
    for(auto pos : state2) {
      EXPECT_NE(15-pos.first, pos.second);
      on_count++;
    }
    EXPECT_EQ(on_count, 240);
    for(unsigned int row = 0; row < 16; row++) {
      EXPECT_EQ(state1.get_row(row), 0x00ffffu & ~((MatrixState<16,16>::row_t)1<<row));
      EXPECT_EQ(state2.get_row(row), 0x00ffffu & ~((MatrixState<16,16>::row_t)1<<(15-row)));
    }
    EXPECT_NE(state1, state2);
  }

  TEST(MatrixStateTest, CompareMatrix) {
    MatrixState<16,16> state1, state2;
    // Set up state1 with every other entry on
    for(unsigned int row = 0; row < 16; row++) {
      state1.write_row(row, (0x00a5a5u << (row%2)));
    }
    EXPECT_NE(state1, state2);
    state2 = state1;
    // After assignment there should be no differences
    EXPECT_EQ(state1, state2);
    MatrixState<16,16> diff = state1 ^ state2;
    unsigned int diff_count = 0;
    for(auto pos : diff) {
      diff_count++;
    }
    EXPECT_EQ(diff_count, 0);
    // Flip the diagonal entries
    for(unsigned int row = 0; row < 16; row++) {
      state2.write_row(row, state2.get_row(row) ^ (1<<row));
    }
    EXPECT_NE(state1, state2);
    // The difference should be exactly the diagonal entires
    diff_count = 0;
    for(auto pos : state1 ^ state2) {
      EXPECT_EQ(pos.first, pos.second);
      diff_count++;
    }
    EXPECT_EQ(diff_count, 16);
    state1 = state2;
    // After another assignment there should be no differences
    EXPECT_EQ(state1, state2);
    diff_count = 0;
    for(auto pos : state1 ^ state2) {
      diff_count++;
    }
    EXPECT_EQ(diff_count, 0);
  }

}
// ----------------------------------------------------------------------
// Parameterized tests for different matrix sizes
// ----------------------------------------------------------------------
namespace {
  typedef ::testing::Types<
    // These are matrix dimensions more like a keyboard taken from the
    // TMK firmware: 1x1, 5x14, 6x4, 6x17, 6x18, 8x18, 9x7, 9x8, 9x10,
    // 11x8, 12x8, 14x6, 16x8, 18x8
    MatrixState<5,14>,
    MatrixState<6,4>,
    MatrixState<6,17>,
    MatrixState<6,18>,
    MatrixState<8,18>,
    MatrixState<9,7>,
    MatrixState<9,8>,
    MatrixState<9,10>,
    MatrixState<11,8>,
    MatrixState<12,8>,
    MatrixState<14,6>,
    MatrixState<16,8>,
    MatrixState<18,8>,

    // These test different boundary conditions on column counts which
    // affect the typedef of the integer used for each row
    MatrixState<8,7>,
    MatrixState<8,8>,
    MatrixState<8,9>,
    MatrixState<16,15>,
    MatrixState<16,16>,
    MatrixState<16,17>,
    MatrixState<32,31>,
    MatrixState<32,32>,
    MatrixState<32,33>
    > MatrixSizeTypes;

  template <typename T>
  class MatrixSizeTest : public ::testing::Test {
  };

  TYPED_TEST_CASE(MatrixSizeTest, MatrixSizeTypes);
  // Test that an empty matrix is, in fact, empty
  TYPED_TEST(MatrixSizeTest, EmptyMatrix) {
    TypeParam state;
    EXPECT_TRUE(state.begin() == state.end());
    unsigned int on_count = 0;
    for(auto pos : state) {
      on_count++;
    }
    EXPECT_EQ(on_count, 0);
    for(unsigned int row = 0; row < TypeParam::row_count; row++) {
      EXPECT_EQ(state.get_row(row), 0);
    }
    for(unsigned int row = 0; row < TypeParam::row_count; row++) {
      for(unsigned int col = 0; col < TypeParam::col_count; col++) {
	EXPECT_FALSE(state.get(row,col));
      }
    }
  }
  // Construct a diagonal matrix state and check it
  TYPED_TEST(MatrixSizeTest, DiagMatrix) {
    TypeParam state1, state2;
    for(unsigned int i = 0; i < TypeParam::row_count; i++) {
      // When we have more rows than columns, wrap around after
      // exhausting all the columns
      state1.set(i, i % TypeParam::col_count);
      // Reverse diagonal
      state2.set(i, TypeParam::col_count - 1 - (i % TypeParam::col_count));
    }
    unsigned int on_count = 0;
    for(auto pos : state1) {
      EXPECT_EQ(pos.first % TypeParam::col_count, pos.second);
      on_count++;
    }
    EXPECT_EQ(on_count, TypeParam::row_count);
    on_count = 0;
    for(auto pos : state2) {
      EXPECT_EQ(TypeParam::col_count - 1 - (pos.first % TypeParam::col_count),
		pos.second);
      on_count++;
    }
    EXPECT_EQ(on_count, TypeParam::row_count);
    for(unsigned int row = 0; row < TypeParam::row_count; row++) {
      EXPECT_EQ(state1.get_row(row), ((typename TypeParam::row_t) 1)<<(row % TypeParam::col_count));
      EXPECT_EQ(state2.get_row(row), ((typename TypeParam::row_t) 1)<<(TypeParam::col_count - 1 - (row % TypeParam::col_count)));
    }
    EXPECT_NE(state1, state2);
  }

  // Construct an inverse diagonal matrix (diagonals off, everything
  // else on)
  TYPED_TEST(MatrixSizeTest, InverseDiagMatrix) {
    TypeParam state1, state2;
    for(unsigned int i = 0; i < TypeParam::row_count; i++) {
      state1.write_row(i, ~(typename TypeParam::row_t)0);
      state1.clear(i, i % TypeParam::col_count);
      state2.write_row(i, ~(typename TypeParam::row_t)0);
      state2.clear(i, TypeParam::col_count - 1 - (i % TypeParam::col_count));
    }
    unsigned int on_count = 0;
    for(auto pos : state1) {
      EXPECT_NE(pos.first % TypeParam::col_count, pos.second);
      on_count++;
    }
    EXPECT_EQ(on_count, (TypeParam::row_count * TypeParam::col_count) - TypeParam::row_count);
    on_count = 0;
    for(auto pos : state2) {
      EXPECT_NE(TypeParam::col_count - 1 - (pos.first % TypeParam::col_count), pos.second);
      on_count++;
    }
    EXPECT_EQ(on_count, (TypeParam::row_count * TypeParam::col_count) - TypeParam::row_count);
    for(unsigned int row = 0; row < TypeParam::row_count; row++) {
      typename TypeParam::row_t row_mask = ((~(typename TypeParam::row_t)0)
					    >>((sizeof(typename TypeParam::row_t) * 8) - TypeParam::col_count));
      EXPECT_EQ(state1.get_row(row), row_mask & ~((typename TypeParam::row_t)1<<(row % TypeParam::col_count)));
      EXPECT_EQ(state2.get_row(row), row_mask & ~((typename TypeParam::row_t)1<<(TypeParam::col_count - 1
										 - (row % TypeParam::col_count))));
    }
    EXPECT_NE(state1, state2);
  }

  TYPED_TEST(MatrixSizeTest, CompareMatrix) {
    TypeParam state1, state2;
    // Set up state1 with every other entry on
    typename TypeParam::row_t alternating_bits = 1;
    for(unsigned int col = 0 ; col < TypeParam::col_count; col+=2) {
      alternating_bits |= alternating_bits << col;
    }
    for(unsigned int row = 0; row < TypeParam::row_count; row++) {
      state1.write_row(row, (alternating_bits << (row%2)));
    }
    EXPECT_NE(state1, state2);
    state2 = state1;
    // After assignment there should be no differences
    EXPECT_EQ(state1, state2);
    TypeParam diff = state1 ^ state2;
    unsigned int diff_count = 0;
    for(auto pos : diff) {
      diff_count++;
    }
    EXPECT_EQ(diff_count, 0);
    // Flip the diagonal entries
    for(unsigned int row = 0; row < TypeParam::row_count; row++) {
      state2.write_row(row, state2.get_row(row) ^ ((typename TypeParam::row_t)1<<(row % TypeParam::col_count)));
    }
    EXPECT_NE(state1, state2);
    // The difference should be exactly the diagonal entires
    diff_count = 0;
    for(auto pos : state1 ^ state2) {
      EXPECT_EQ(pos.first % TypeParam::col_count, pos.second);
      diff_count++;
    }
    EXPECT_EQ(diff_count, TypeParam::row_count);
    state1 = state2;
    // After another assignment there should be no differences
    EXPECT_EQ(state1, state2);
    diff_count = 0;
    for(auto pos : state1 ^ state2) {
      diff_count++;
    }
    EXPECT_EQ(diff_count, 0);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
