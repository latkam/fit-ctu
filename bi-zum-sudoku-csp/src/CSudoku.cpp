#include "CSudoku.hpp"

std::vector<CSudoku::TFill> CSudoku::s_Fills = std::vector<CSudoku::TFill> ( );
CNTree<CSudoku> CSudoku::s_Tree = CNTree<CSudoku> ( );
bool CSudoku::s_Solved = false;
CNTree<CSudoku>::CNode * CSudoku::s_SolvedSudoku = nullptr;
int CSudoku::s_Iterations = 0;
//----------------------------------------------------------------------------------------------------------------------
CSudoku::TCell::TCell ( int dimensions, int x, int y, int value ) :
  m_Dimensions ( dimensions ),
  m_X          ( x ),
  m_Y          ( y ),
  m_Value      ( value )
{
  for ( int i = 1; i <= dimensions; ++ i )
  {
    if ( i == value )
      continue;
    m_PossibleValues . insert ( i );
  }
}
//----------------------------------------------------------------------------------------------------------------------
std::ostream & operator << ( std::ostream & os, const CSudoku::TCell & x )
{
  os << "V ";
  if ( x . m_Value == 0 && x . m_PossibleValues . size ( ) == 1 )
  {
    os << "\033[31m" << x . m_Value << "\033[0m" << " ";
    os << "PV " << "\033[34m";
    for ( const auto & v : x . m_PossibleValues )
      os << v;
    os << "\033[0m";
  }
  else
  {
    os << x . m_Value;
    if ( x . m_Value / 10 == 0 )
      os << " ";
    os << "PV ";
    for ( const auto & v : x . m_PossibleValues )
      os << v;
  }
  for ( int i = 0; i < x . m_Dimensions; ++ i )
  {
    if ( x . m_PossibleValues . find ( i + 1 ) == x . m_PossibleValues . end ( ) )
      for ( int j = 0; j < ( ( i + 1 ) / 10 ) + 1; ++ j )
        os << " ";
  }
  return os;
}
//----------------------------------------------------------------------------------------------------------------------
bool CSudoku::TCellCmp::operator ( ) ( const TCell * a, const TCell * b )
{
  return a -> m_PossibleValues . size ( ) > b -> m_PossibleValues . size ( );
}
//----------------------------------------------------------------------------------------------------------------------
CSudoku::TFill::TFill ( void ) :
  m_X     ( 0 ),
  m_Y     ( 0 ),
  m_Value ( 0 ) { }
//----------------------------------------------------------------------------------------------------------------------
CSudoku::TFill::TFill ( int x, int y, int value ) :
  m_X     ( x ),
  m_Y     ( y ),
  m_Value ( value ) { }
//----------------------------------------------------------------------------------------------------------------------
std::ostream & operator << ( std::ostream & os, const CSudoku::TFill & x )
{
  os << "[" << x . m_X << "," << x . m_Y << "], value: " << x . m_Value;
  return os;
}
//----------------------------------------------------------------------------------------------------------------------
CSudoku::CSudoku ( int inDimensionsW, int inDimensionsH, int dimensions ) :
    m_InDimensionsW ( inDimensionsW ),
    m_InDimensionsH ( inDimensionsH ),
    m_Dimensions    ( dimensions )
{
  InitialiseCells ( );
}
//----------------------------------------------------------------------------------------------------------------------
bool CSudoku::LoadSudoku ( const std::string fileName )
{
  int input;
  std::ifstream in ( fileName );
  if ( ! in . is_open ( ) )
    return false;
  in >> m_Dimensions >> m_InDimensionsW >> m_InDimensionsH;
  if ( m_Dimensions == 0 )
  {
    m_Dimensions = 9;
    m_InDimensionsW = m_InDimensionsH = 3;
  }
  if ( m_Dimensions != 9 )
    InitialiseCells ( );
  for ( int y_iter = 0; y_iter < m_Dimensions; ++ y_iter )
    for ( int x_iter = 0; x_iter < m_Dimensions; ++ x_iter )
    {
      in >> input;
      SetCell ( x_iter, y_iter, input );
    }
  in . close ( );
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CSudoku::SolveSudokuManual ( std::ostream & os )
{
  s_Fills . clear ( );
  int x, y, value;
  os << "Solve Sudoku." << std::endl;
  os << *this;
  while ( true )
  {
    os << "Enter x, y and value:";
    std::cin >> x >> y >> value;
    switch ( SetCell ( x, y, value ) )
    {
      case 0:
        os << "\033[31mFilled value does not satisfy constraints.\033[0m" << std::endl;
        break;
      case 1:
        os << "\033[32mFilled value satisfies constraints. \033[0m" << std::endl;
        break;
      case 2:
        os << "\033[32mFilled value satisfies constraints and the sudoku is solved." << std::endl;
        os << *this;
        return;
      default:
        break;
    }
    os << *this;
    os << "\033[0m";
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CSudoku::SolveSudokuHalfAutomatic ( std::ostream & os )
{
  s_Fills . clear ( );
  os << "Solving Sudoku." << std::endl;
  os << *this;
  while ( FillDeterministicCell ( ) ) { }
  for ( const auto & f : s_Fills )
    os << f << std::endl;
  os << *this;
  if ( IsSolved ( ) )
    os << "\033[32mSudoku solved.\033[0m" << std::endl;
  else
    os << "\033[31mSudoku not solved.\033[0m" << std::endl;
}
//----------------------------------------------------------------------------------------------------------------------
void CSudoku::SolveSudokuAutomatic ( std::ostream & os )
{
  s_Tree . SetRoot ( *this );
  CNTree<CSudoku>::CNode ** ptr = s_Tree . GetRoot ( );
  if ( s_Solved )
  {
    s_Solved = true;
    s_SolvedSudoku = *ptr;
  }
  else
  {
    TCell * tileToFill = GetBestFromAll ( );
    if ( tileToFill )
      (*ptr) -> m_V . FillCell ( ptr, tileToFill -> m_X, tileToFill -> m_Y );
  }
  if ( ! s_Solved )
  {
    os << "\033[31mSudoku not solved. Performed " << s_Iterations << " iterations.\033[0m" << std::endl;
    return;
  }
  else
    ReturnToRoot ( s_SolvedSudoku );
  std::reverse ( s_Fills . begin ( ), s_Fills . end ( ) );
  for ( auto & fill : s_Fills )
    os << fill << std::endl;
  os << s_SolvedSudoku -> m_V;
  os << "\033[32mSudoku solved after " << s_Iterations << " iterations.\033[0m" << std::endl;
}
//----------------------------------------------------------------------------------------------------------------------
int CSudoku::SetCell ( int x, int y, int value )
{
  if ( x >= m_Dimensions || y >= m_Dimensions )
    throw std::invalid_argument ( "Tile position out of bounds." );
  if ( value < 0 || value > m_Dimensions )
    throw std::invalid_argument ( "Invalid tile value." );

  if ( m_Cells[y][x] . m_Value != 0 )
    IncreasePossibleValues ( x, y, m_Cells[y][x] . m_Value );
  m_Cells[y][x] . m_Value = value;

  if ( m_Cells[y][x] . m_PossibleValues . find ( value )  == m_Cells[y][x] . m_PossibleValues . end ( ) )
  {
    ReducePossibleValues ( x, y, value );
    return 0;
  }

  ReducePossibleValues ( x, y, value );
  if ( IsSolved ( ) )
    return 2;
  if ( SatisfiesConstraints ( x, y ) )
    return 1;
  return 0;
}
//----------------------------------------------------------------------------------------------------------------------
size_t CSudoku::NmbToSet ( void ) const
{
  size_t res = 0;
  for ( const auto & r : m_Cells )
    res += std::count_if ( r . begin ( ),
                           r . end   ( ),
                           [] ( const TCell & c ) { return c . m_Value == 0; } );
  return res;
}
//----------------------------------------------------------------------------------------------------------------------
bool CSudoku::SatisfiesConstraints ( int x, int y )
{
  if ( m_Cells[y][x] . m_Value == 0 )
    return true;

  /* Check row. */
  std::set<TCell *> row = GetDependentRow ( y );
  for ( auto cell : row )
  {
    if ( cell -> m_X == x )
      continue;
    if ( cell -> m_Value == m_Cells[y][x] . m_Value )
      return false;
  }
  /* Check column. */
  std::set<TCell *> column = GetDependentColumn ( x );
  for ( auto cell : column )
  {
    if ( cell -> m_Y == y )
      continue;
    if ( cell -> m_Value == m_Cells[y][x] . m_Value )
      return false;
  }

  /* Check "square". */
  std::set<TCell *> square = GetDependentSquare ( x, y );
  for ( auto cell : square )
  {
    if ( cell -> m_X == x && cell -> m_Y == y )
      continue;
    if ( cell -> m_Value == m_Cells[y][x] . m_Value )
      return false;
  }
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CSudoku::SatisfiesConstraints ( void )
{
  for ( int y = 0; y < m_Dimensions; ++ y )
    for ( int x = 0; x < m_Dimensions; ++ x )
      if ( ! SatisfiesConstraints ( x, y ) )
        return false;
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CSudoku::IsSolved ( void )
{
  for ( int y = 0; y < m_Dimensions; ++ y )
    for ( int x = 0; x < m_Dimensions; ++ x )
      if ( m_Cells[y][x] . m_PossibleValues . size ( ) > 0
        || m_Cells[y][x] . m_Value == 0
        || ! SatisfiesConstraints ( x, y ) )
        return false;
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
std::ostream & operator << ( std::ostream & os, const CSudoku & x )
{
  x . PrintHorSep ( os );
  for ( int y_iter = 0; y_iter < x . m_Dimensions; ++ y_iter )
  {
    x . PrintHorSepSpace ( os );
    x . PrintHorSepSpace ( os );
    os << '|';
    for ( int x_iter = 0; x_iter < x . m_Dimensions; ++ x_iter )
      os << x . m_Cells[y_iter][x_iter] << '|';
    os << std::endl;
    x . PrintHorSepSpace ( os );
    x . PrintHorSepSpace ( os );
    x . PrintHorSep      ( os );
  }
  return os;
}
//----------------------------------------------------------------------------------------------------------------------
void CSudoku::IncreasePossibleValues ( int x, int y, int value )
{
  if ( value == 0 )
    throw std::invalid_argument ( "Cannot increase possible values with value 0." );
  std::set<TCell *> dependents = GetDependents ( x, y );
  for ( auto cell : dependents )
    cell -> m_PossibleValues . insert ( value );
}
//----------------------------------------------------------------------------------------------------------------------
void CSudoku::ReducePossibleValues ( int x, int y, int value )
{
  std::set<TCell *> dependents = GetDependents ( x, y );
  for ( auto cell : dependents )
    cell -> m_PossibleValues . erase ( value );
}
//----------------------------------------------------------------------------------------------------------------------
bool CSudoku::FillDeterministicCell ( void )
{
  for ( int y = 0; y < m_Dimensions; ++ y )
    for ( int x = 0; x < m_Dimensions; ++ x )
      if ( m_Cells[y][x] . m_Value == 0 && m_Cells[y][x] . m_PossibleValues . size ( ) == 1 )
      {
        s_Fills . emplace_back ( x, y, *m_Cells[y][x] . m_PossibleValues . begin ( ) );
        SetCell ( x, y, *m_Cells[y][x] . m_PossibleValues . begin ( ) );
        return true;
      }
  return false;
}
//----------------------------------------------------------------------------------------------------------------------
void CSudoku::InitialiseCells ( void )
{
  m_Cells . clear ( );

  for ( int y = 0; y < m_Dimensions; ++ y )
  {
    std::vector<TCell> row;
    for ( int x = 0; x < m_Dimensions; ++ x )
      row . emplace_back ( m_Dimensions, x, y, 0 );
    m_Cells . emplace_back ( row  );
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CSudoku::FillCell ( CNTree <CSudoku>::CNode ** ptr, int x, int y )
{
  ++ s_Iterations;
  /* Assign filled values to child nodes. */
  std::vector<CSudoku> childrenWithFilledValues;
  for ( auto possibleVal : (*ptr) -> m_V . m_Cells[y][x] . m_PossibleValues )
  {
    CSudoku newSudoku ( (*ptr) -> m_V );
    newSudoku . m_FilledTile = TFill ( x, y, possibleVal );
    newSudoku . SetCell ( x, y, possibleVal );
    childrenWithFilledValues . emplace_back ( newSudoku );
  }
  (*ptr) -> AddChildren ( childrenWithFilledValues );

  /* Check if any child is solved sudoku. */
  for ( size_t i = 0; i < (*ptr) -> m_Ch . size ( ); ++ i )
    if ( (*ptr) -> m_Ch[i] -> m_V . IsSolved ( ) )
    {
      s_Solved = true;
      s_SolvedSudoku = (*ptr) -> m_Ch[i];
      return;
    }

  /* Find value for all children satisfying constraints and call FillCell on them. */
  for ( size_t i = 0; i < (*ptr) -> m_Ch . size ( ); ++ i )
  {
    if ( ! s_Solved && (*ptr) -> m_Ch[i] -> m_V . SatisfiesConstraints ( ) )
    {
      TCell * tileToFill = (*ptr) -> m_Ch[i] -> m_V . GetBestFromDependents ( x, y );
      if ( ! tileToFill )
        tileToFill = (*ptr) -> m_Ch[i] -> m_V . GetBestFromAll ( );
      if ( ! tileToFill )
        return;
      (*ptr) -> m_Ch[i] -> m_V . FillCell ( & (*ptr) -> m_Ch[i], tileToFill -> m_X, tileToFill -> m_Y );
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
std::set<CSudoku::TCell *> CSudoku::GetDependents ( int x, int y )
{
  std::set<TCell *> row    = GetDependentRow    ( y );
  std::set<TCell *> column = GetDependentColumn ( x );
  std::set<TCell *> square = GetDependentSquare ( x, y );
  row . merge ( column );
  row . merge ( square );
  return row;
}
//----------------------------------------------------------------------------------------------------------------------
std::set<CSudoku::TCell *> CSudoku::GetDependentRow ( int y )
{
  std::set<TCell *> res;
  for ( int x_iter = 0; x_iter < m_Dimensions; ++ x_iter )
    res . insert ( & m_Cells[y][x_iter] );
  return res;
}
//----------------------------------------------------------------------------------------------------------------------
std::set<CSudoku::TCell *> CSudoku::GetDependentColumn ( int x )
{
  std::set<TCell *> res;
  for ( int y_iter = 0; y_iter < m_Dimensions; ++ y_iter )
    res . insert ( & m_Cells[y_iter][x] );
  return res;
}
//----------------------------------------------------------------------------------------------------------------------
std::set<CSudoku::TCell *> CSudoku::GetDependentSquare ( int x, int y )
{
  std::set<TCell *> res;
  int square_idx_min_x = x - x % m_InDimensionsW;
  int square_idx_max_x = square_idx_min_x + m_InDimensionsW; /* <begin; end) */
  int square_idx_min_y = y - y % m_InDimensionsH;
  int square_idx_max_y = square_idx_min_y + m_InDimensionsH; /* <begin; end) */

  for ( int x_iter = square_idx_min_x; x_iter < square_idx_max_x; ++ x_iter )
    for ( int y_iter = square_idx_min_y; y_iter < square_idx_max_y; ++ y_iter )
      res . insert ( & m_Cells[y_iter][x_iter] );
  return res;
}
//----------------------------------------------------------------------------------------------------------------------
CSudoku::TCell * CSudoku::GetBestFromDependents ( int x, int y )
{
  std::set<TCell *> dependents = GetDependents ( x, y );

  while ( ! m_ToFill . empty ( ) )
    m_ToFill . pop ( );

  for ( auto d : dependents )
    if ( d -> m_Value == 0 && ! d -> m_PossibleValues . empty ( ) )
      m_ToFill . push ( d );

  if ( m_ToFill . empty ( ) )
    return nullptr;
  return m_ToFill . top ( );
}
//----------------------------------------------------------------------------------------------------------------------
CSudoku::TCell * CSudoku::GetBestFromAll ( void )
{
  while ( ! m_ToFill . empty ( ) )
    m_ToFill . pop ( );

  for ( auto & r : m_Cells )
    for ( auto & c : r )
      if ( c . m_Value == 0 && ! c . m_PossibleValues . empty ( ) )
        m_ToFill . push ( & c );

  if ( m_ToFill . empty ( ) )
    return nullptr;
  return m_ToFill . top ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CSudoku::ReturnToRoot ( CNTree<CSudoku>::CNode * node )
{
  s_Fills . clear ( );
  CNTree<CSudoku>::CNode ** ptr = & node;
  while ( (*ptr) -> m_P )
  {
    s_Fills . emplace_back ( node -> m_V . m_FilledTile );
    (*ptr) = (*ptr) -> m_P;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CSudoku::PrintHorSepSpace ( std::ostream & os ) const
{
  int cell_width = 7;
  for ( int i = 0; i < m_Dimensions; ++ i )
    cell_width += ( ( i + 1 ) / 10 ) + 1;
  os << "|";
  for ( int x_iter = 0; x_iter < m_Dimensions; ++ x_iter )
  {
    for ( int i = 0; i < cell_width; ++ i )
      os << " ";
    os << "|";
  }
  os << std::endl;
}
//----------------------------------------------------------------------------------------------------------------------
void CSudoku::PrintHorSep ( std::ostream & os ) const
{
  int cell_width = 7;
  for ( int i = 0; i < m_Dimensions; ++ i )
    cell_width += ( ( i + 1 ) / 10 ) + 1;
  os << '+';
  for ( int i = 0; i < m_Dimensions; ++ i )
  {
    for ( int j = 0; j < cell_width; ++ j )
      os << '-';
    os << '+';
  }
  os << std::endl;
}
//----------------------------------------------------------------------------------------------------------------------
