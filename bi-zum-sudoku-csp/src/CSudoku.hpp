#pragma once

#include "CNTree.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <stdexcept>

/**
 * @class CSudoku
 * @brief Represents sudoku.
 */
class CSudoku
{
public:
                          CSudoku                  ( int inDimensionsW = 3, int inDimensionsH = 3, int dimensions = 9 );

  /**
   * @note Cannot detect invalid input data format.
   */
  bool                    LoadSudoku               ( const std::string fileName );
  void                    SolveSudokuManual        ( std::ostream & os );
  void                    SolveSudokuHalfAutomatic ( std::ostream & os );
  void                    SolveSudokuAutomatic     ( std::ostream & os );
  /**
   * Sets cell.
   * @param value value to be filled.
   * @return 0=filled value does not satisfy constraints,
   *         1=filled value satisfies constraints,
   *         2=filled value satisties constraints and the sudoku is solved.
   */
  int                     SetCell                  ( int x, int y, int value );
  size_t                  NmbToSet                 ( void ) const;
  bool                    SatisfiesConstraints     ( int x, int y );
  bool                    SatisfiesConstraints     ( void );
  bool                    IsSolved                 ( void );
  friend std::ostream   & operator <<              ( std::ostream & os, const CSudoku & x );

  /**
   * @struct TCell
   * @brief Represets one cell in the Sudoku table.
   */
  struct TCell
  {
                          TCell                    ( int dimensions, int x, int y, int value );
    friend std::ostream & operator <<              ( std::ostream & os, const TCell & x );

    int           m_Dimensions;
    int           m_X;
    int           m_Y;
    int           m_Value;
    std::set<int> m_PossibleValues;
  };

  /**
   * @struct TCellCmp
   * @brief Compares pointered instances of TCell.
   */
  struct TCellCmp
  {
    /**
     * @brief Operator performing the comparison.
     * @return true=*a has more possible values than *b,
     *         false=*a has less or equal possible values than *b
     */
    bool operator ( ) ( const TCell * a, const TCell * b );
  };

  struct TFill
  {
                          TFill                    ( void );
                          TFill                    ( int x, int y, int value );
    friend std::ostream & operator <<              ( std::ostream & os, const TFill & x );

    int m_X;
    int m_Y;
    int m_Value;
  };
private:
  void                    IncreasePossibleValues   ( int x, int y, int value );
  void                    ReducePossibleValues     ( int x, int y, int value );

  /**
   * @brief Fills one unfilled cell with deterministic (one) possible value.
   * @return true=filled one cell,
   *         false=not filled anything (no such cell available)
   */
  bool                    FillDeterministicCell    ( void );

  /**
   * @brief Fills one unfilled cell having least possible values with all possible values.
   * @return std::vector of new instances of CSudoku with filled cell.
   */
  void                    InitialiseCells          ( void );
  void                    FillCell                 ( CNTree <CSudoku>::CNode ** ptr, int x, int y );
  std::set<TCell *>       GetDependents            ( int x, int y );
  std::set<TCell *>       GetDependentRow          ( int y );
  std::set<TCell *>       GetDependentColumn       ( int x );
  std::set<TCell *>       GetDependentSquare       ( int x , int y );
  TCell *                 GetBestFromDependents    ( int x,  int y );
  TCell *                 GetBestFromAll           ( void );
  void                    ReturnToRoot             ( CNTree<CSudoku>::CNode * node );
  void                    PrintHorSepSpace         ( std::ostream & os ) const;
  void                    PrintHorSep              ( std::ostream & os ) const;

  int                                                          m_InDimensionsW;
  int                                                          m_InDimensionsH;
  int                                                          m_Dimensions;
  std::vector<std::vector<TCell>>                              m_Cells;
  static std::vector<TFill>                                    s_Fills;
  std::priority_queue<TCell *, std::vector<TCell *>, TCellCmp> m_ToFill;
  TFill                                                        m_FilledTile;
  static CNTree<CSudoku>                                       s_Tree;
  static bool                                                  s_Solved;
  static CNTree<CSudoku>::CNode *                              s_SolvedSudoku;
  static int                                                   s_Iterations;
};