#include "CBlinky.hpp"

//----------------------------------------------------------------------------------------------------------------------
CBlinky::CBlinky ( CGameBoard         & board,
                   int                & PMDir,
                   CGameBoard::TCoord & PMPos,
                   int                & blDir,
                   CGameBoard::TCoord & blPos ) :
  CCharacter ( board,
               PMDir,
               PMPos,
               blDir,
               blPos,
               board . GetGhScttrTmr ( ),
               board . GetGhChaseTmr ( ),
               NConfig::GHFRGHTTMR )
{
  m_Pos = board . GetGhStart ( )[0];
  srand ( time ( NULL ) );
}
//----------------------------------------------------------------------------------------------------------------------
void CBlinky::ToFrght ( void )
{
  CGameBoard::TBoardTile * currTile = m_Board . GetCurrTile ( m_Pos );
  currTile -> m_R = CGameBoard::ETileVal::BLINKY_FRGHT;
  m_ChgTiles . emplace_back ( currTile );
}
//----------------------------------------------------------------------------------------------------------------------
void CBlinky::FromFrght ( void )
{
  CGameBoard::TBoardTile * currTile = m_Board . GetCurrTile ( m_Pos );
  currTile -> m_R = CGameBoard::ETileVal::BLINKY;
  m_ChgTiles . emplace_back ( currTile );
}
//----------------------------------------------------------------------------------------------------------------------
int CBlinky::MoveScttr ( void )
{
  return MoveChase ( );
}
//----------------------------------------------------------------------------------------------------------------------
int CBlinky::MoveChase ( void )
{
  if ( ( m_Iter ++ ) % m_ChaseTmr )
    return NConfig::NO_CHANGE;
  CGameBoard::TBoardTile * currTile = m_Board . GetCurrTile ( m_Pos ),
                         * nextTile = nullptr,
                         * nextTeleport = nullptr;
  nextTile = m_Board . GetBestNextTile ( m_Pos, m_Dir, m_PMPos );
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
      if ( currTile -> m_V == CGameBoard::ETileVal::BLINKY )
        currTile -> m_V = currTile -> m_R = CGameBoard::ETileVal::FRESH;
      else
        currTile -> m_R = currTile -> m_V;
      nextTeleport = m_Board . GetNextTeleport ( nextTile -> m_Crd );
      m_Pos = nextTeleport -> m_Crd;
      nextTeleport -> m_R = CGameBoard::ETileVal::BLINKY;
      m_ChgTiles . emplace_back ( currTile );
      m_ChgTiles . emplace_back ( nextTeleport );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::PACMAN:
      Step ( currTile, nextTile );
      return NConfig::PACMAN_EATEN;
    case CGameBoard::ETileVal::PINKY:
    case CGameBoard::ETileVal::INKY:
    case CGameBoard::ETileVal::CLYDE:
      TurnBack ( );
      return NConfig::NO_CHANGE;
    default:
      break;
  }
  return NConfig::NO_CHANGE;
}
//----------------------------------------------------------------------------------------------------------------------
int CBlinky::MoveFrght ( void )
{
  if ( ( m_Iter ++ ) % m_FrghtTmr )
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
    return 0;

  switch ( nextTile -> m_R )
  {
    case CGameBoard::ETileVal::FRESH:
    case CGameBoard::ETileVal::DOT:
    case CGameBoard::ETileVal::BONUS:
    case CGameBoard::ETileVal::CHERRY:
      Step ( currTile, nextTile, true );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::TELEPORT:
      if ( currTile -> m_V == CGameBoard::ETileVal::BLINKY
        || currTile -> m_V == CGameBoard::ETileVal::BLINKY_FRGHT )
        currTile -> m_V = currTile -> m_R = CGameBoard::ETileVal::FRESH;
      else
        currTile -> m_R = currTile -> m_V;
      nextTeleport = m_Board . GetNextTeleport ( nextTile -> m_Crd );
      m_Pos = nextTeleport -> m_Crd;
      nextTeleport -> m_R = CGameBoard::ETileVal::BLINKY_FRGHT;
      m_ChgTiles . emplace_back ( currTile );
      m_ChgTiles . emplace_back ( nextTeleport );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::PACMAN:
      Step ( currTile, nextTile, true );
      return NConfig::BLINKY_EATEN;
    case CGameBoard::ETileVal::PINKY_FRGHT:
    case CGameBoard::ETileVal::INKY_FRGHT:
    case CGameBoard::ETileVal::CLYDE_FRGHT:
      TurnBack ( );
      return NConfig::NO_CHANGE;
    default:
      break;
  }
  return NConfig::NO_CHANGE;
}
//----------------------------------------------------------------------------------------------------------------------
void CBlinky::Step ( CGameBoard::TBoardTile * currTile, CGameBoard::TBoardTile * nextTile, bool frght )
{
  switch ( currTile -> m_V )
  {
    case CGameBoard::ETileVal::BLINKY:
    case CGameBoard::ETileVal::BLINKY_FRGHT:
      currTile -> m_V = currTile -> m_R = CGameBoard::ETileVal::FRESH;
      break;  
    case CGameBoard::ETileVal::TELEPORT:
      currTile -> m_R = CGameBoard::ETileVal::TELEPORT;
      break;
    default:
      currTile -> m_R = currTile -> m_V;
  }
  frght ? nextTile -> m_R = CGameBoard::ETileVal::BLINKY_FRGHT : nextTile -> m_R = CGameBoard::ETileVal::BLINKY;
  m_ChgTiles . emplace_back ( currTile );
  m_ChgTiles . emplace_back ( nextTile );
  m_Pos = nextTile -> m_Crd;
}
//----------------------------------------------------------------------------------------------------------------------
