#if !defined(_MATRIX_STATE_HPP)
#define _MATRIX_STATE_HPP

#include "bit_width.hpp"
#include <utility>
#include <assert.h>

// A matrix state is made up of a number of rows each with a set of
// bits representing the columns.
template<unsigned int _row_n, unsigned int _col_n>
class MatrixState
{
public:
  // One row in the matrix
  typedef typename BitWidth<_col_n>::uint row_t;
  // A position in the matrix: a row/column pair
  typedef typename std::pair<unsigned int, unsigned int> pos_t;

  // row/column counts for cases where they may not be known
  static const unsigned int row_count;
  static const unsigned int col_count;
private:
  row_t rows[_row_n];
  static row_t valid_column_mask() noexcept;
public:
  MatrixState();
  MatrixState(const MatrixState<_row_n,_col_n> &other);

  // Read the state of a single row/column position
  bool get(unsigned int row, unsigned int col) const noexcept;
  // Get an entire row
  row_t get_row(unsigned int row) const noexcept;
  // Set the bit at the specified row/column position
  void set(unsigned int row, unsigned int col) noexcept;
  // Clear the bit at the specified row/column position
  void clear(unsigned int row, unsigned int col) noexcept;
  // Write an entire row
  void write_row(unsigned int row, row_t col_data) noexcept;

  MatrixState<_row_n,_col_n> &operator=(const MatrixState<_row_n,_col_n> &other) noexcept;

  // Useful for comparing two matix states
  MatrixState<_row_n,_col_n> operator^(const MatrixState<_row_n,_col_n> &other) const noexcept;

  bool operator==(const MatrixState<_row_n,_col_n> &other) const noexcept;
  bool operator!=(const MatrixState<_row_n,_col_n> &other) const noexcept
  { return !(*this == other); }

  // Iterator over the row/column pairs which are set (i.e. keys which
  // are pressed) in a state
  class iterator
  {
  private:
    // The first element of the pair is the row, the second is the column
    pos_t cur;
    // Move to the next element in the row, wrapping to the next row
    // if needed
    void incr() noexcept;
    // Find the next set element (i.e. pressed key) in the matrix
    void find_next() noexcept;
  public:
    const MatrixState<_row_n,_col_n> &state;
    iterator(const MatrixState<_row_n,_col_n> &state,
	     unsigned int row = 0, unsigned int col = 0)
      : state(state), cur(row, col)
    { find_next(); }

    // The * and -> operators give the current row/column pair
    const pos_t &operator*() const  { return cur; }
    const pos_t *operator->() const { return &cur; }
    bool operator==(const iterator &other) const noexcept;
    bool operator!=(const iterator &other) const { return !(*this == other); }
    const iterator &operator++() { incr(); find_next(); return *this; }
  };

  // STL-style begin/end iterator generators to support the C++11
  // range for syntax.
  iterator begin() const noexcept { return iterator(*this); }
  iterator end() const noexcept { return iterator(*this, _row_n); }
};

// ----------------------------------------------------------------------
// Implementation
// ----------------------------------------------------------------------

template<unsigned int _row_n, unsigned int _col_n>
const unsigned int MatrixState<_row_n,_col_n>::row_count = _row_n;
template<unsigned int _row_n, unsigned int _col_n>
const unsigned int MatrixState<_row_n,_col_n>::col_count = _col_n;

template<unsigned int _row_n, unsigned int _col_n>
MatrixState<_row_n,_col_n>::MatrixState() {
  for(unsigned int i = 0; i < _row_n; i++) {
    rows[i] = 0;
  }
}

template<unsigned int _row_n, unsigned int _col_n>
MatrixState<_row_n,_col_n>::MatrixState(const MatrixState<_row_n,_col_n> &other) {
  for(unsigned int i = 0; i < _row_n; i++) {
    rows[i] = other.rows[i];
  }
}

template<unsigned int _row_n, unsigned int _col_n>
bool MatrixState<_row_n,_col_n>::get(unsigned int row, unsigned int col) const noexcept {
  if((row >= _row_n) || (col >= _col_n))
    return false;
  return ((rows[row]>>col & 1) == 1);
}

template<unsigned int _row_n, unsigned int _col_n>
typename MatrixState<_row_n,_col_n>::row_t MatrixState<_row_n,_col_n>::get_row(unsigned int row) const noexcept {
  if(row >= _row_n)
    return 0;
  return rows[row];
}

template<unsigned int _row_n, unsigned int _col_n>
void MatrixState<_row_n,_col_n>::set(unsigned int row, unsigned int col) noexcept {
  if((row < _row_n) && (col < _col_n)) {
    rows[row] |= ((row_t)1)<<col;
  }
}

template<unsigned int _row_n, unsigned int _col_n>
void MatrixState<_row_n,_col_n>::clear(unsigned int row, unsigned int col) noexcept {
  if((row < _row_n) && (col < _col_n)) {
    rows[row] &= ~(((row_t)1)<<col);
      }
}

template<unsigned int _row_n, unsigned int _col_n>
void MatrixState<_row_n,_col_n>::write_row(unsigned int row,
					   typename MatrixState<_row_n,_col_n>::row_t col_data) noexcept {
  if(row < _row_n)
    rows[row] = col_data & valid_column_mask();
}

template<unsigned int _row_n, unsigned int _col_n>
MatrixState<_row_n,_col_n> &MatrixState<_row_n,_col_n>::operator=(const MatrixState<_row_n,_col_n> &other) noexcept {
  for(unsigned int i = 0; i < _row_n; i++) {
    rows[i] = other.rows[i];
  }
  return *this;
}

template<unsigned int _row_n, unsigned int _col_n>
typename MatrixState<_row_n,_col_n>::row_t MatrixState<_row_n,_col_n>::valid_column_mask() noexcept {
  return (~(row_t)0)>>((sizeof(row_t) * 8) - _col_n);
}

template<unsigned int _row_n, unsigned int _col_n>
MatrixState<_row_n,_col_n> MatrixState<_row_n,_col_n>::operator^(const MatrixState<_row_n,_col_n> &other) const noexcept {
  MatrixState<_row_n,_col_n> result;
  for(unsigned int i = 0; i < _row_n; i++) {
    result.rows[i] = rows[i] ^ other.rows[i];
  }
  return result;
}

template<unsigned int _row_n, unsigned int _col_n>
bool MatrixState<_row_n,_col_n>::operator==(const MatrixState<_row_n,_col_n> &other) const noexcept {
  for(unsigned int i = 0; i < _row_n; i++) {
    if(rows[i] != other.rows[i]) {
      return false;
    }
  }
  return true;
}

template<unsigned int _row_n, unsigned int _col_n>
void MatrixState<_row_n,_col_n>::iterator::incr() noexcept {
  cur.second++;
  if(cur.second >= _col_n) {
    cur.first += cur.second / _col_n;
    cur.second %= _col_n;
  }
}

template<unsigned int _row_n, unsigned int _col_n>
void MatrixState<_row_n,_col_n>::iterator::find_next() noexcept {
  while(cur.first < _row_n) {
    if(state.rows[cur.first] == 0) {
      // Nothing in this row: move to the next row
      cur.first++;
      cur.second = 0;
    }
    else if(!(state.rows[cur.first]>>cur.second & 1)) {
      // Nothing in this column: move on to the next column,
      // wrapping to the next row as needed
      incr();
    }
    else
      // Found it!
      break;
  }
}

template<unsigned int _row_n, unsigned int _col_n>
bool MatrixState<_row_n,_col_n>::iterator::operator==(const iterator &other)
  const noexcept {
  if(cur.first >= _row_n)
    // Any past-the-end iterator is equal to any other
    // past-the-end iterator
    return other.cur.first >= _row_n;
  // Otherwise the current row/column must match.
  return other.cur == cur;
}

#endif
