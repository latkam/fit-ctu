#include "CSudoku.hpp"

#include <iostream>
#include <cstring>


int main ( int argc, char * argv[] )
{
  if ( argc != 3 )
    std::cerr << "usage sudoku <filename> <manual/half-automatic/automatic>" << std::endl;

  CSudoku sudoku;
  sudoku . LoadSudoku ( argv[1] );
  if ( sudoku . SatisfiesConstraints ( ) )
    std::cout << "\033[32mLoaded sudoku satisfies constraints.\033[0m" << std::endl;
  else
  {
    std::cerr << "\033[31mLoaded sudoku does not satisfy constraints.\033[0m" << std::endl;
    return 1;
  }
  if ( ! strcmp ( argv[2], "manual" ) )
    sudoku . SolveSudokuManual ( std::cout );
  else if ( ! strcmp ( argv[2], "half-automatic" ) )
    sudoku . SolveSudokuHalfAutomatic ( std::cout );
  else if ( ! strcmp ( argv[2], "automatic" ) )
    sudoku . SolveSudokuAutomatic ( std::cout );
  else
    std::cerr << "usage sudoku <filename> <manual/half-automatic/automatic>" << std::endl;
  return 0;
}