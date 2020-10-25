#include "include/logger/logger.hpp"
#include "matrix/matrix.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
  try {
    Matrix<int, 3> matrix{};

    for (int x{}, y{ 9'999'999 }, z{ 9'999'999 }; x < 10'000'000; ++x, --y, --z) {
      matrix[x][y][z] = z;
      matrix[z][x][y] = y;
      matrix[y][z][x] = x;
    }    
    
    auto a = matrix[0][9][9];
    LOG(a, matrix[1][8][8]);
    LOG(matrix[100][200][300]);

    for (const auto& matrix_element : matrix) {
      LOG(matrix_element);
    }

  } catch (const std::exception& e) {
    ERROR(e.what());
  }

  return 0;
}
