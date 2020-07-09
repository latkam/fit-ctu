#include "CInky.hpp"

//----------------------------------------------------------------------------------------------------------------------
CInky::CInky ( CGameBoard         & board,
               int                & PMDir,
               CGameBoard::TCoord & PMPos,
               int                & blDir,
               CGameBoard::TCoord & blPos ) :
  CCharacter    ( board,
                  PMDir,
                  PMPos,
                  blDir,
                  blPos,
                  board . GetGhScttrTmr ( ),
                  board . GetGhChaseTmr ( ),
                  NConfig::GHFRGHTTMR ),
  m_ScttrCorner ( CGameBoard::TCoord ( m_Board . GetMaxX ( ), 0 ) )
{
  m_Pos = board . GetGhStart ( )[2];
}
//----------------------------------------------------------------------------------------------------------------------
void CInky::ToFrght ( void )
{
  CGameBoard::TBoardTile * currTile = m_Board . GetCurrTile ( m_Pos );
  currTile -> m_R = CGameBoard::ETileVal::INKY_FRGHT;
  m_ChgTiles . emplace_back ( currTile );
}
//----------------------------------------------------------------------------------------------------------------------
void CInky::FromFrght ( void )
{
  CGameBoard::TBoardTile * currTile = m_Board . GetCurrTile ( m_Pos );
  currTile -> m_R = CGameBoard::ETileVal::INKY;
  m_ChgTiles . emplace_back ( currTile );
}
//----------------------------------------------------------------------------------------------------------------------
int CInky::MoveScttr ( void )
{
  if ( m_InHouse && m_Iter > NConfig::INKY_DELAY && ( m_LHRet = LeaveHouse ( CGameBoard::ETileVal::INKY, CGameBoard::ETileVal::INKY_FRGHT ) ) )
    return m_LHRet;
  if ( ( m_Iter ++ ) % m_ScttrTmr )
    return NConfig::NO_CHANGE;
  if ( m_InHouse )
    return NConfig::NO_CHANGE;
  CGameBoard::TBoardTile * currTile = m_Board . GetCurrTile ( m_Pos ),
                         * nextTile = nullptr,
                         * nextTeleport = nullptr;
  nextTile = m_Board . GetBestNextTile ( m_Pos, m_Dir, m_ScttrCorner );
  if ( ! nextTile )
    return NConfig::NO_CHANGE;

  switch ( nextTile -> m_R )
  {
    case CGameBoard::ETileVal::FRESH:
    case CGameBoard::ETileVal::DOT:
    case CGameBoard::ETileVal::BONUS:
    case CGameBoard::ETileVal::CHERRY:
      Step ( currTile, nextTile );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::TELEPORT:
      if ( currTile -> m_V == CGameBoard::ETileVal::INKY )
        currTile -> m_V = currTile -> m_R = CGameBoard::ETileVal::FRESH;
      else
        currTile -> m_R = currTile -> m_V;
      nextTeleport = m_Board . GetNextTeleport ( nextTile -> m_Crd );
      m_Pos = nextTeleport -> m_Crd;
      nextTeleport -> m_R = CGameBoard::ETileVal::INKY;
      m_ChgTiles . emplace_back ( currTile );
      m_ChgTiles . emplace_back ( nextTeleport );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::PACMAN:
      Step ( currTile, nextTile );
      return NConfig::PACMAN_EATEN;
    case CGameBoard::ETileVal::BLINKY:
    case CGameBoard::ETileVal::PINKY:
    case CGameBoard::ETileVal::CLYDE:
      TurnBack ( );
      return NConfig::NO_CHANGE;
    default:
      break;
  }
  return NConfig::NO_CHANGE;
}
//----------------------------------------------------------------------------------------------------------------------
int CInky::MoveChase ( void )
{
  if ( m_InHouse && m_Iter > NConfig::INKY_DELAY && ( m_LHRet = LeaveHouse ( CGameBoard::ETileVal::INKY, CGameBoard::ETileVal::INKY_FRGHT ) ) )
    return m_LHRet;
  if ( ( m_Iter ++ ) % m_ChaseTmr )
    return NConfig::NO_CHANGE;
  if ( m_InHouse )
    return NConfig::NO_CHANGE;
  CGameBoard::TBoardTile * currTile = m_Board . GetCurrTile ( m_Pos ),
      * nextTeleport = nullptr,
      * beforePM = m_Board . GetTileDistance ( m_PMPos, m_PMDir, NConfig::INKY_TARGET_DIST, NConfig::INKY_TARGET_DIST );

  int diffX = abs ( beforePM -> m_Crd . m_X - m_BlPos . m_X );
  int diffY = abs ( beforePM -> m_Crd . m_Y - m_BlPos . m_Y );

  CGameBoard::TBoardTile * targetTile = m_Board . GetTileDistance ( m_BlPos, m_BlDir, diffX * 2, diffY * 2, false ),
                         * nextTile = m_Board . GetBestNextTile ( m_Pos, m_Dir, targetTile -> m_Crd );

  if ( ! nextTile )
    return NConfig::NO_CHANGE;

  switch ( nextTile -> m_R )
  {
    case CGameBoard::ETileVal::FRESH:
    case CGameBoard::ETileVal::DOT:
    case CGameBoard::ETileVal::BONUS:
    case CGameBoard::ETileVal::CHERRY:
      Step ( currTile, nextTile );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::TELEPORT:
      if ( currTile -> m_V == CGameBoard::ETileVal::INKY )
        currTile -> m_V = currTile -> m_R = CGameBoard::ETileVal::FRESH;
      else
        currTile -> m_R = currTile -> m_V;
      nextTeleport = m_Board . GetNextTeleport ( nextTile -> m_Crd );
      m_Pos = nextTeleport -> m_Crd;
      nextTeleport -> m_R = CGameBoard::ETileVal::INKY;
      m_ChgTiles . emplace_back ( currTile );
      m_ChgTiles . emplace_back ( nextTeleport );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::PACMAN:
      Step ( currTile, nextTile );
      return NConfig::PACMAN_EATEN;
    case CGameBoard::ETileVal::BLINKY:
    case CGameBoard::ETileVal::PINKY:
    case CGameBoard::ETileVal::CLYDE:
      TurnBack ( );
      return NConfig::NO_CHANGE;
    default:
      break;
  }
  return NConfig::NO_CHANGE;
}
//----------------------------------------------------------------------------------------------------------------------
int CInky::MoveFrght ( void )
{
  if ( m_InHouse && m_Iter > NConfig::INKY_DELAY && ( m_LHRet = LeaveHouse ( CGameBoard::ETileVal::INKY, CGameBoard::ETileVal::INKY_FRGHT, true ) ) )
    return m_LHRet;
  if ( ( m_Iter ++ ) % m_FrghtTmr )
    return NConfig::NO_CHANGE;
  if ( m_InHouse )
    return NConfig::NO_CHANGE;
  CGameBoard::TBoardTile * currTile = m_Board . GetCurrTile ( m_Pos ),
      * nextTile = nullptr,
      * nextTeleport = nullptr;

  if ( m_Board . IsCrossing ( m_Pos, m_Dir ) )
  {
    m_Dir = NConfig::Mod ( rand ( ) );
    nextTile = m_Board . GetNextTile ( m_Pos, m_Dir );
  }
  else
    nextTile = m_Board . GetNextTile ( m_Pos, NConfig::Mod ( m_Dir ) );
  if ( ! nextTile )
    return NConfig::NO_CHANGE;

  switch ( nextTile -> m_R )
  {
    case CGameBoard::ETileVal::FRESH:
    case CGameBoard::ETileVal::DOT:
    case CGameBoard::ETileVal::BONUS:
    case CGameBoard::ETileVal::CHERRY:
      Step ( currTile, nextTile, true );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::TELEPORT:
      if ( currTile -> m_V == CGameBoard::ETileVal::INKY
           || currTile -> m_V == CGameBoard::ETileVal::INKY_FRGHT )
        currTile -> m_V = currTile -> m_R = CGameBoard::ETileVal::FRESH;
      else
        currTile -> m_R = currTile -> m_V;
      nextTeleport = m_Board . GetNextTeleport ( nextTile -> m_Crd );
      m_Pos = nextTeleport -> m_Crd;
      nextTeleport -> m_R = CGameBoard::ETileVal::INKY_FRGHT;
      m_ChgTiles . emplace_back ( currTile );
      m_ChgTiles . emplace_back ( nextTeleport );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::PACMAN:
      Step ( currTile, nextTile, true );
      return NConfig::INKY_EATEN;
    case CGameBoard::ETileVal::BLINKY_FRGHT:
    case CGameBoard::ETileVal::PINKY_FRGHT:
    case CGameBoard::ETileVal::CLYDE_FRGHT:
      TurnBack ( );
      return NConfig::NO_CHANGE;
    default:
      break;
  }
  return 0;
}
//----------------------------------------------------------------------------------------------------------------------
void CInky::Step ( CGameBoard::TBoardTile * currTile, CGameBoard::TBoardTile * nextTile, bool frght )
{
  switch ( currTile -> m_V )
  {
    case CGameBoard::ETileVal::INKY:
    case CGameBoard::ETileVal::INKY_FRGHT:
      currTile -> m_V = currTile -> m_R = CGameBoard::ETileVal::FRESH;
      break;
    case CGameBoard::ETileVal::TELEPORT:
      currTile -> m_R = CGameBoard::ETileVal::TELEPORT;
      break;
    default:
      currTile -> m_R = currTile -> m_V;
  }
  frght ? nextTile -> m_R = CGameBoard::ETileVal::INKY_FRGHT : nextTile -> m_R = CGameBoard::ETileVal::INKY;
  m_ChgTiles . emplace_back ( currTile );
  m_ChgTiles . emplace_back ( nextTile );
  m_Pos = nextTile -> m_Crd;
}
//----------------------------------------------------------------------------------------------------------------------
