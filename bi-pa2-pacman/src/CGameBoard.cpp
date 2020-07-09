#include "CGameBoard.hpp"

//----------------------------------------------------------------------------------------------------------------------
CGameBoard::CGameBoard ( void ) :
    m_MaxX        ( 0 ),
    m_MaxY        ( 0 ),
    m_PacManSet   ( false ),
    m_CherrySet   ( false ),
    m_ScttrTmr    ( 1 ),
    m_ChaseTmr    ( 1 ),
    m_FrghtTmr    ( 1 ),
    m_GhScttrTmr  ( 1 ),
    m_GhChaseTmr  ( 1 )
{
  m_GhostStart . resize ( NConfig::NMB_GHOSTS );
}
//----------------------------------------------------------------------------------------------------------------------
CGameBoard::TCoord::TCoord ( int x, int y ) :
  m_X ( x ),
  m_Y ( y ) { }
//----------------------------------------------------------------------------------------------------------------------
bool CGameBoard::TCoord::operator == ( const TCoord & x ) const { return m_X == x . m_X && m_Y == x . m_Y; }
//----------------------------------------------------------------------------------------------------------------------
bool CGameBoard::TCoord::operator!= ( const TCoord & x ) const { return ! ( *this == x ); }
//----------------------------------------------------------------------------------------------------------------------
CGameBoard::TBoardTile::TBoardTile ( int x, int y, char value ) :
    m_Crd ( x, y )
{
  switch ( value )
  {
    case '+':
      m_V = ETileVal::WALL_CORNER;
      break;
    case '-':
      m_V = ETileVal::WALL_HORIZONTAL;
      break;
    case '|':
      m_V = ETileVal::WALL_VERTICAL;
      break;
    case '=':
      m_V = ETileVal::DOOR;
      break;  
    case ' ':
      m_V = ETileVal::FRESH;
      break;
    case 'C':
      m_V = ETileVal::PACMAN;
      break;
    case 'B':
      m_V = ETileVal::BLINKY_FRGHT;
      break;
    case 'P':
      m_V = ETileVal::PINKY_FRGHT;
      break;
    case 'I':
      m_V = ETileVal::INKY_FRGHT;
      break;
    case 'L':
      m_V = ETileVal::CLYDE_FRGHT;
      break;
    case 'b':
      m_V = ETileVal::BLINKY;
      break;
    case 'p':
      m_V = ETileVal::PINKY;
      break;
    case 'i':
      m_V = ETileVal::INKY;
      break;
    case 'c':
      m_V = ETileVal::CLYDE;
      break;
    case '.':
      m_V = ETileVal::DOT;
      break;
    case 'o':
      m_V = ETileVal::BONUS;
      break;
    case '%':
      m_V = ETileVal::CHERRY;
      break;
    case 'T':
      m_V = ETileVal::TELEPORT;
      break;
    default:
      break;
  }
  m_R = m_V;
}
//----------------------------------------------------------------------------------------------------------------------
void CGameBoard::TBoardTile::Render ( WINDOW * win, int y, int x, bool small ) const
{
  switch ( m_R )
  {
    case ETileVal::WALL_CORNER:
      wattron   ( win, COLOR_PAIR ( 4 ) );
      mvwprintw ( win, y, x, "+" );
      wattroff  ( win, COLOR_PAIR ( 4 ) );
      break;
    case ETileVal::WALL_HORIZONTAL:
      wattron   ( win, COLOR_PAIR ( 4 ) );
      mvwprintw ( win, y, x, "-" );
      wattroff  ( win, COLOR_PAIR ( 4 ) );
      break;
    case ETileVal::WALL_VERTICAL:
      wattron   ( win, COLOR_PAIR ( 4 ) );
      mvwprintw ( win, y, x, "|" );
      wattroff  ( win, COLOR_PAIR ( 4 ) );
      break;
    case ETileVal::DOOR:
      wattron   ( win, COLOR_PAIR ( 3 ) );
      mvwprintw ( win, y, x, "=" );
      wattroff  ( win, COLOR_PAIR ( 3 ) );
      break;
    case ETileVal::PACMAN:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 3 ) );
      small ? mvwprintw ( win, y, x, "c" ) : mvwprintw ( win, y, x, "C" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 3 ) );
      break;
    case ETileVal::BLINKY_FRGHT:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 4 ) );
      mvwprintw ( win, y, x, "A" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 4 ) );
      break;
    case ETileVal::PINKY_FRGHT:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 4 ) );
      mvwprintw ( win, y, x, "A" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 4 ) );
      break;
    case ETileVal::INKY_FRGHT:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 4 ) );
      mvwprintw ( win, y, x, "A" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 4 ) );
      break;
    case ETileVal::CLYDE_FRGHT:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 4 ) );
      mvwprintw ( win, y, x, "A" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 4 ) );
      break;
    case ETileVal::BLINKY:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 1 ) );
      mvwprintw ( win, y, x, "A" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 1 ) );
      break;
    case ETileVal::PINKY:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 6 ) );
      mvwprintw ( win, y, x, "A" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 6 ) );
      break;
    case ETileVal::INKY:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 5 ) );
      mvwprintw ( win, y, x, "A" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 5 ) );
      break;
    case ETileVal::CLYDE:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 7 ) );
      mvwprintw ( win, y, x, "A" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 7 ) );
      break;
    case ETileVal::FRESH:
      mvwprintw ( win, y, x, " " );
      break;
    case ETileVal::DOT:
      wattron   ( win, COLOR_PAIR ( 3 ) );
      mvwprintw ( win, y, x, "." );
      wattroff  ( win, COLOR_PAIR ( 3 ) );
      break;
    case ETileVal::BONUS:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 3 ) );
      mvwprintw ( win, y, x, ".");
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 3 ) );
      break;
    case ETileVal::CHERRY:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 1 ) );
      mvwprintw ( win, y, x , "%%" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 1 ) );
      break;
    case ETileVal::TELEPORT:
      wattron   ( win, A_BOLD | COLOR_PAIR ( 4 ) );
      mvwprintw ( win, y, x, "T" );
      wattroff  ( win, A_BOLD | COLOR_PAIR ( 4 ) );
      break;
  }
}
//----------------------------------------------------------------------------------------------------------------------
bool CGameBoard::LoadBoardData ( const char * fileName, WINDOW * win )
{
  char c;
  int x = 0, y = 0;
  std::ifstream in ( NConfig::DEFAULT_DIR + "/" + std::string ( fileName ) );
  std::string dummy1, dummy2, difficulty, delimiter;
  
  
  if ( ! in . is_open ( ) )
  {
    mvwprintw ( win, 5, 1, NConfig::ERROR_CANNOT_OPEN );
    return false;
  }

  /* Load maze. */
  in >> std::noskipws;
  m_Tiles . emplace_back ( std::vector<TBoardTile> ( ) );
  while ( in >> c && c != NConfig::STOP_INPUT_CHAR )
  {
    if ( NConfig::POSS_TILES . find ( c ) == NConfig::POSS_TILES . end ( ) )
       return ErrorMessage ( win, in, NConfig::ERROR_INVALID_TILE );
     
    if ( c == '\n' )
    {
      m_Tiles . emplace_back ( std::vector<TBoardTile> ( ) );
      x = 0;
      ++ y;
      continue;
    }

    m_Tiles[y] . emplace_back ( TBoardTile ( x, y, c ) );
    if ( ! StorePosInfo ( m_Tiles[y][x] ) )
      return ErrorMessage ( win, in, NConfig::ERROR_PACMAN_SETUP );
    ++ x;   
  }
  m_Tiles . erase ( m_Tiles . end ( ) - 1 );

  m_MaxY = m_Tiles . size ( ) - 1;
  m_MaxY > -1 ? m_MaxX = m_Tiles[0] . size ( ) - 1 : m_MaxX = -1;

  if ( ! in . good ( ) )
    return ErrorMessage ( win, in, NConfig::ERROR_CANNOT_READ );
  if ( m_Teleports . size ( ) != NConfig::TELEPORTS )
    return ErrorMessage ( win, in, NConfig::ERROR_INVALID_TELEPORT );
  
  /* Load game and ghost difficulty. */
  in >> std::skipws;
  in >> dummy1 >> dummy2 >> difficulty;
  std::transform ( difficulty . begin ( ), difficulty . end ( ), difficulty . begin ( ),
                 [] ( unsigned char c ) { return std::tolower ( c ); } );
                 
  if ( ! in . good ( )
    || dummy1 != NConfig::INPUT_GAME
    || dummy2 != NConfig::INPUT_DIFFICULTY
    || ( difficulty != NConfig::DIFFICULTY_EASY
      && difficulty != NConfig::DIFFICULTY_MEDIUM
      && difficulty != NConfig::DIFFICULTY_HARD ) )
    return ErrorMessage ( win, in, NConfig::INPUT_GAME_DIFF_FAIL );

  if ( difficulty == NConfig::DIFFICULTY_EASY )
    m_GameDiff = EDifficulty::EASY;
  else if ( difficulty == NConfig::DIFFICULTY_MEDIUM )
    m_GameDiff = EDifficulty::MEDIUM;
  else
    m_GameDiff = EDifficulty::HARD;

  in >> dummy1 >> dummy2 >> difficulty;
  std::transform ( difficulty . begin ( ), difficulty . end ( ), difficulty . begin ( ),
                   [] ( unsigned char c ) { return std::tolower ( c ); } );
  if ( ! in . good ( )
    || dummy1 != NConfig::INPUT_GHOST
    || dummy2 != NConfig::INPUT_DIFFICULTY
    || ( difficulty != NConfig::DIFFICULTY_EASY
      && difficulty != NConfig::DIFFICULTY_MEDIUM
      && difficulty != NConfig::DIFFICULTY_HARD ) )
    return ErrorMessage ( win, in, NConfig::INPUT_GHOST_DIFF_FAIL );
    
  if ( difficulty == NConfig::DIFFICULTY_EASY )
    m_GhostDiff = EDifficulty::EASY;
  else if ( difficulty == NConfig::DIFFICULTY_MEDIUM )
    m_GhostDiff = EDifficulty::MEDIUM;
  else
    m_GhostDiff = EDifficulty::HARD;
  if ( ! m_PacManSet
       || m_GhostSet . size ( ) != NConfig::NMB_GHOSTS
       || ! m_CherrySet
    || ! CheckBoardIntegrity ( ))
    return ErrorMessage ( win, in, NConfig::ERROR_INTEGRITY );
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CGameBoard::SetGameAttr ( void )
{
  /* Set up timers for characters. */
  if ( m_GhostDiff == EDifficulty::EASY )
  {
    m_GhScttrTmr = 3;
    m_GhChaseTmr = 3;
  }
  else if ( m_GhostDiff == EDifficulty::MEDIUM )
    m_GhScttrTmr = m_GhChaseTmr = 2;
  else
  {
    m_GhScttrTmr = 1;
    m_GhChaseTmr = 2;
  }

  /** Set up timers for game modes. */
  if ( m_GameDiff == EDifficulty::EASY )
  {
    m_ScttrTmr  = 3;
    m_ChaseTmr  = 7;
    m_FrghtTmr  = 5;
  }
  else if ( m_GhostDiff == EDifficulty::MEDIUM )
  {
    m_ScttrTmr  = 2;
    m_ChaseTmr  = 9;
    m_FrghtTmr  = 3;
  }
  else
  {
    m_ScttrTmr  = 1;
    m_ChaseTmr  = 10;
    m_FrghtTmr  = 1;
  }
}
//----------------------------------------------------------------------------------------------------------------------
size_t CGameBoard::GetNmbToEat ( void ) const { return m_Dots . size ( ) + m_Bonuses . size ( ); }
//----------------------------------------------------------------------------------------------------------------------
CGameBoard::TCoord & CGameBoard::GetPacManStart ( void ) { return m_PacManStart; }
//----------------------------------------------------------------------------------------------------------------------
std::vector<CGameBoard::TCoord> & CGameBoard::GetGhStart ( void ) { return m_GhostStart; }
//----------------------------------------------------------------------------------------------------------------------
int CGameBoard::GetScttrTmr ( void ) const { return m_ScttrTmr; }
//----------------------------------------------------------------------------------------------------------------------
int CGameBoard::GetChaseTmr ( void ) const { return m_ChaseTmr; }
//----------------------------------------------------------------------------------------------------------------------
int CGameBoard::GetFrghtTmr ( void ) const { return m_FrghtTmr; }
//----------------------------------------------------------------------------------------------------------------------
int CGameBoard::GetGhScttrTmr ( void ) const { return m_GhScttrTmr; }
//----------------------------------------------------------------------------------------------------------------------
int CGameBoard::GetGhChaseTmr ( void ) const { return m_GhChaseTmr; }
//----------------------------------------------------------------------------------------------------------------------
int CGameBoard::GetMaxX ( void ) const { return m_MaxX; }
//----------------------------------------------------------------------------------------------------------------------
int CGameBoard::GetMaxY ( void ) const { return m_MaxY; }
//----------------------------------------------------------------------------------------------------------------------
double CGameBoard::EclDist ( const TCoord & a, const TCoord & b ) const
{
  return sqrt ( pow ( b . m_X - a . m_X, 2 ) + pow ( b . m_Y - a . m_Y, 2 ) );
}
//----------------------------------------------------------------------------------------------------------------------
void CGameBoard::Render ( WINDOW * win, int offsetX, int offsetY ) const
{
  for ( size_t y = 0; y < m_Tiles . size ( ); ++ y )
  {
    for ( size_t x = 0; x < m_Tiles[y] . size ( ); ++ x )
      m_Tiles[y][x] . Render ( win, y + offsetY + NConfig::INWINSHIFTH, x + offsetX + NConfig::INWINSHIFTW );
  }
  wrefresh ( win );
}
//----------------------------------------------------------------------------------------------------------------------
void CGameBoard::CleanForNewGame ( bool eaten )
{
  /* Set original Fresh tiles. */
  for ( const auto & f : m_Freshes )
    m_Tiles[f . m_Y][f . m_X] . m_V = m_Tiles[f . m_Y][f . m_X] . m_R = ETileVal::FRESH;

  /* Set original Dots. */
  for ( const auto & d : m_Dots )
    m_Tiles[d . m_Y][d . m_X] . m_V = m_Tiles[d . m_Y][d . m_X] . m_R = ETileVal::DOT;

  /* Set eaten dots to Fresh. */
  if ( eaten )
    for ( const auto & de : m_DotsEaten )
      m_Tiles[de . m_Y][de . m_X] . m_V = m_Tiles[de . m_Y][de . m_X] . m_R = ETileVal::FRESH;
  else
    m_DotsEaten . clear ( );

  /* Set original Bonuses. */
  for ( const auto & b : m_Bonuses )
    m_Tiles[b . m_Y][b . m_X] . m_V = m_Tiles[b . m_Y][b . m_X] . m_R = ETileVal::BONUS;

  /* Set eaten Bonuses to Fresh. */
  if ( eaten )
    for ( const auto & be : m_BonusesEaten )
      m_Tiles[be . m_Y][be . m_X] . m_V = m_Tiles[be . m_Y][be . m_X] . m_R = ETileVal::FRESH;
  else
    m_BonusesEaten . clear ( );

  /* Set original Teleports. */
  for ( const auto & t : m_Teleports )
    m_Tiles[t . m_Y][t . m_X] . m_V = m_Tiles[t . m_Y][t . m_X] . m_R = ETileVal::TELEPORT;

  /* Set original Cherry. */
  m_Tiles[m_CherryPos . m_Y][m_CherryPos . m_X] . m_R =
  m_Tiles[m_CherryPos . m_Y][m_CherryPos . m_X] . m_V = ETileVal::CHERRY;

  /* Set original PacMan. */
  m_Tiles[m_PacManStart . m_Y][m_PacManStart . m_X] . m_R =
  m_Tiles[m_PacManStart . m_Y][m_PacManStart . m_X] . m_V = ETileVal::PACMAN;

  /* Set original ghosts */
  m_Tiles[m_GhostStart[0] . m_Y][m_GhostStart[0] . m_X] . m_R =
  m_Tiles[m_GhostStart[0] . m_Y][m_GhostStart[0] . m_X] . m_V = ETileVal::BLINKY;
  m_Tiles[m_GhostStart[1] . m_Y][m_GhostStart[1] . m_X] . m_R =
  m_Tiles[m_GhostStart[1] . m_Y][m_GhostStart[1] . m_X] . m_V = ETileVal::PINKY;
  m_Tiles[m_GhostStart[2] . m_Y][m_GhostStart[2] . m_X] . m_R =
  m_Tiles[m_GhostStart[2] . m_Y][m_GhostStart[2] . m_X] . m_V = ETileVal::INKY;
  m_Tiles[m_GhostStart[3] . m_Y][m_GhostStart[3] . m_X] . m_R =
  m_Tiles[m_GhostStart[3] . m_Y][m_GhostStart[3] . m_X] . m_V = ETileVal::CLYDE;
}
//----------------------------------------------------------------------------------------------------------------------
void CGameBoard::SetCherry ( WINDOW * win, int offsetX, int offsetY )
{
  m_Tiles[m_CherryPos . m_Y][m_CherryPos . m_X] . m_V =
  m_Tiles[m_CherryPos . m_Y][m_CherryPos . m_X] . m_R = ETileVal::CHERRY;
  m_Tiles[m_CherryPos . m_Y][m_CherryPos . m_X] . Render ( win,
                                                           offsetY + NConfig::INWINSHIFTH + m_CherryPos . m_Y,
                                                           offsetX + NConfig::INWINSHIFTW + m_CherryPos . m_X );

  wrefresh ( win );
}
//----------------------------------------------------------------------------------------------------------------------
CGameBoard::TBoardTile * CGameBoard::GetCurrTile ( const TCoord & position )
{
  return & m_Tiles[position . m_Y][position . m_X];
}
//----------------------------------------------------------------------------------------------------------------------
CGameBoard::TBoardTile * CGameBoard::GetNextTile ( const TCoord & position, int direction )
{
  switch ( direction )
  {
    case NConfig::DIR_UP:
      return & m_Tiles[position . m_Y - 1][position . m_X];
    case NConfig::DIR_RIGHT:
      return & m_Tiles[position . m_Y][position . m_X + 1];
    case NConfig::DIR_DOWN:
      return & m_Tiles[position . m_Y + 1][position . m_X];
    case NConfig::DIR_LEFT:
      return & m_Tiles[position . m_Y][position . m_X - 1];
    default:
      throw std::invalid_argument ( NConfig::EXCP_INVALID_DIR );
  }
}
//----------------------------------------------------------------------------------------------------------------------
CGameBoard::TBoardTile * CGameBoard::GetBestNextTile ( const TCoord & position, int & direction, const TCoord & target )
{
  CGameBoard::TBoardTile * tmp = nullptr;
  std::vector<std::pair<int, CGameBoard::TBoardTile *>> nextTiles;
  for ( int i = 0; i < NConfig::DIRECTIONS; ++ i )
  {
    if ( i == 2 )
      continue;
    tmp = GetNextTile ( position, NConfig::Mod ( direction + i ) );
    if ( tmp -> m_R != ETileVal::WALL_CORNER
      && tmp -> m_R != ETileVal::WALL_HORIZONTAL
      && tmp -> m_R != ETileVal::WALL_VERTICAL
      && tmp -> m_R != ETileVal::DOOR )
      nextTiles . emplace_back ( NConfig::Mod ( direction + i ), tmp );
  }
  if ( nextTiles . empty ( ) )
    return nullptr;
  auto res = ( std::min_element ( nextTiles . begin ( ),
                                  nextTiles . end ( ),
                                  [&] ( const std::pair<int, TBoardTile *> a, const std::pair<int, TBoardTile *> b )
                                  { return EclDist ( a . second -> m_Crd, target ) <
                                           EclDist ( b . second -> m_Crd, target ); } ) );
  direction = res -> first;
  return res -> second;
}
//----------------------------------------------------------------------------------------------------------------------
CGameBoard::TBoardTile * CGameBoard::GetTileDistance ( const TCoord & position, int direction, int diffX, int diffY, bool regardDirection )
{
  if ( regardDirection )
    switch ( direction )
    {
      case NConfig::DIR_UP:
        return & m_Tiles[NConfig::Mod ( position . m_Y - diffY, m_MaxY + 1 )][NConfig::Mod ( position . m_X, m_MaxX + 1 )];
      case NConfig::DIR_RIGHT:
        return & m_Tiles[NConfig::Mod ( position . m_Y, m_MaxY + 1 )][NConfig::Mod ( position . m_X + diffX, m_MaxX + 1 )];
      case NConfig::DIR_DOWN:
        return & m_Tiles[NConfig::Mod ( position . m_Y + diffY, m_MaxY + 1 )][NConfig::Mod ( position . m_X, m_MaxX + 1 )];
      case NConfig::DIR_LEFT:
        return & m_Tiles[NConfig::Mod ( position . m_Y, m_MaxY + 1 )][NConfig::Mod ( position . m_X - diffX, m_MaxX + 1 )];
      default:
        throw std::invalid_argument ( NConfig::EXCP_INVALID_DIR );
    }
  else
    return & m_Tiles[NConfig::Mod ( position . m_Y + diffY, m_MaxY + 1 )][NConfig::Mod ( position . m_X + diffX, m_MaxX + 1 )];

}
//----------------------------------------------------------------------------------------------------------------------
CGameBoard::TBoardTile * CGameBoard::GetNextTeleport ( const TCoord & currPos )
{
  if ( m_Teleports . size ( ) < 2 )
    throw std::logic_error ( NConfig::EXCP_FEW_TELEPORTS );
  for ( size_t i = 0; i < m_Teleports . size ( ); ++ i )
  {
    if ( currPos == m_Teleports[i] )
      return & m_Tiles[m_Teleports[( i + 1 ) % m_Teleports . size ( )] . m_Y][m_Teleports[( i + 1 ) % m_Teleports . size ( )] . m_X];
  }
  throw std::invalid_argument ( NConfig::EXCP_UNKNOWN_TELEPORT );
}
//----------------------------------------------------------------------------------------------------------------------
bool CGameBoard::IsCrossing ( const TCoord & position, int direction )
{
  TBoardTile * tmp;
  std::set<int> nextTiles;
  for ( int i = 0; i < NConfig::DIRECTIONS; ++ i )
  {
    tmp = GetNextTile ( position, NConfig::Mod ( direction + i ) );
    if ( tmp -> m_V != ETileVal::WALL_CORNER
      && tmp -> m_V != ETileVal::WALL_HORIZONTAL
      && tmp -> m_V != ETileVal::WALL_VERTICAL
      && tmp -> m_V != ETileVal::DOOR )
      nextTiles . insert ( direction + i );
  }
  if ( nextTiles . size ( ) >= 3 )
    return true;
  return nextTiles . find ( direction + 1 ) != nextTiles . end ( ) || nextTiles . find ( direction + 3 ) != nextTiles . end ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CGameBoard::DotEaten ( const TCoord & position ) { m_DotsEaten . emplace_back ( position ); }
//----------------------------------------------------------------------------------------------------------------------
void CGameBoard::BonusEaten ( const TCoord & position ) { m_BonusesEaten . emplace_back ( position ); }
//----------------------------------------------------------------------------------------------------------------------
bool CGameBoard::ErrorMessage ( WINDOW * win, std::ifstream & in, const char * message )
{
  mvwprintw ( win, 5, 1, message );
  in . close ( );
  return false;
}
//----------------------------------------------------------------------------------------------------------------------
bool CGameBoard::StorePosInfo ( CGameBoard::TBoardTile & tile )
{
  TCoord dummy;
  switch ( tile . m_V )
  {
    case ETileVal::PACMAN:
      if ( ! m_PacManSet )
      {
        m_PacManStart = tile . m_Crd;
        m_PacManSet = true;
      }
      else
        return false;
      break;
    case ETileVal::BLINKY:
      if ( ! GhStartEmpty ( ) || tile . m_Crd . m_Y < NConfig::HOME_MIN_Y )
        return false;
      m_GhostStart[0] = tile . m_Crd;
      m_GhostSet . insert ( tile . m_V );
      break;
    case ETileVal::PINKY:
      if ( m_GhostStart[1] != dummy || ! CheckGhostHome ( tile . m_Crd ) )
        return false;
      m_GhostStart[1] = tile . m_Crd;
      m_GhostSet . insert ( tile . m_V );
      break;
    case ETileVal::INKY:
      if ( m_GhostStart[2] != dummy || ! CheckGhostHome ( tile . m_Crd ) )
        return false;
      m_GhostStart[2] = tile . m_Crd;
      m_GhostSet . insert ( tile . m_V );
      break;
    case ETileVal::CLYDE:
      if ( m_GhostStart[3] != dummy || ! CheckGhostHome ( tile . m_Crd ) )
        return false;
      m_GhostStart[3] = tile . m_Crd;
      m_GhostSet . insert ( tile . m_V );
      break;
    case ETileVal::FRESH:
      m_Freshes . emplace_back ( tile . m_Crd );
      break;
    case ETileVal::DOT:
      m_Dots . emplace_back ( tile . m_Crd );
      break;
    case ETileVal::BONUS:
      m_Bonuses . emplace_back ( tile . m_Crd );
      break;
    case ETileVal::CHERRY:
      if ( ! m_CherrySet )
      {
        m_CherryPos = tile . m_Crd;
        m_CherrySet = true;
      }
      else
        return false;
      break;
    case ETileVal::TELEPORT:
      m_Teleports . emplace_back ( tile . m_Crd );
      break;
    default:
      break;
  }
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CGameBoard::CheckBoardIntegrity ( void )
{
  if ( m_Tiles[0][0]           . m_V != ETileVal::WALL_CORNER
    || m_Tiles[0][m_MaxX]      . m_V != ETileVal::WALL_CORNER
    || m_Tiles[m_MaxY][m_MaxX] . m_V != ETileVal::WALL_CORNER
    || m_Tiles[m_MaxY][0]      . m_V != ETileVal::WALL_CORNER )
    return false;

  for ( int i = 1; i < m_MaxX - 1; ++ i )
    if ( m_Tiles[0][i] . m_V != ETileVal::WALL_HORIZONTAL )
      return false;

  for ( int i = 1; i < m_MaxY - 1; ++ i )
    if ( m_Tiles[i][m_MaxX] . m_V != ETileVal::WALL_VERTICAL )
      return false;

  for ( int i = 1; i < m_MaxX - 1; ++ i )
    if ( m_Tiles[m_MaxY][i] . m_V != ETileVal::WALL_HORIZONTAL )
      return false;

  for ( int i = 1; i < m_MaxY - 1; ++ i )
    if ( m_Tiles[i][0] . m_V != ETileVal::WALL_VERTICAL )
      return false;

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CGameBoard::CheckGhostHome ( const TCoord & position )
{
  return ! ( position . m_Y < NConfig::HOME_MIN_Y
          || m_Tiles[position . m_Y - 1][position . m_X] . m_V != ETileVal::DOOR
          || ( m_Tiles[position . m_Y - 2][position . m_X] . m_V != ETileVal::PACMAN
            && m_Tiles[position . m_Y - 2][position . m_X] . m_V != ETileVal::FRESH
            && m_Tiles[position . m_Y - 2][position . m_X] . m_V != ETileVal::DOT
            && m_Tiles[position . m_Y - 2][position . m_X] . m_V != ETileVal::BONUS
            && m_Tiles[position . m_Y - 2][position . m_X] . m_V != ETileVal::CHERRY ) );
}
//----------------------------------------------------------------------------------------------------------------------
bool CGameBoard::GhStartEmpty ( void )
{
  TCoord dummy = TCoord ( 0, 0 );
  for ( const auto & s : m_GhostStart )
    if ( s != dummy )
      return false;
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
