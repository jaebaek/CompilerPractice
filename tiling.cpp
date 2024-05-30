#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

class Matrix {
  int rows;
  int cols;
  std::vector<std::vector<int>> values;

public:
  Matrix(int r, int c) : rows(r), cols(c) {
    values =
        std::move(std::vector<std::vector<int>>(r, std::vector<int>(c, 0)));
  }

  void setValues(std::vector<std::vector<int>> &&v) { values = std::move(v); }

  Matrix operator*(const Matrix &other) {
    assert(cols == other.rows);
    Matrix result(rows, other.cols);
    auto v = std::move(
        std::vector<std::vector<int>>(rows, std::vector<int>(other.cols, 0)));
#ifdef TILE_SIZE
    // std::cout << TILE_SIZE << std::endl;
    for (int i = 0; i < rows; i += TILE_SIZE) {
      for (int j = 0; j < other.cols; j += TILE_SIZE) {
        for (int k = 0; k < cols; ++k) {
          for (int ii = 0; ii < TILE_SIZE && i+ii < rows; ++ii) {
            for (int jj = 0; jj < TILE_SIZE && j+jj < other.cols; ++jj) {
              int x = i + ii;
              int y = j + jj;
              v[x][y] += values[x][k] * other.values[k][y];
            }
          }
        }
      }
    }
#else
    // std::cout << "no_tile" << std::endl;
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < other.cols; ++j) {
        for (int k = 0; k < cols; ++k) {
          v[i][j] += values[i][k] * other.values[k][j];
        }
      }
    }
#endif
    result.setValues(std::move(v));
    return result;
  }

  void print() {
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        std::cout << values[i][j] << " ";
      }
      std::cout << std::endl;
    }
  }
};

int main(int argc, const char *argv[]) {
  Matrix A(1024, 512);
  auto v = std::move(
      std::vector<std::vector<int>>(1024, std::vector<int>(512, 0)));
  for (int i = 0; i < 1024; ++i) {
    for (int j = 0; j < 512; ++j) {
      v[i][j] = i % 5 + j % 3;
    }
  }
  A.setValues(std::move(v));

  Matrix B(512, 768);
  v = std::move(
      std::vector<std::vector<int>>(512, std::vector<int>(768, 0)));
  for (int i = 0; i < 512; ++i) {
    for (int j = 0; j < 768; ++j) {
      v[i][j] = i % 4 + j % 7;
    }
  }
  B.setValues(std::move(v));

  Matrix C = A * B;
  // C.print();
  return 0;
}
