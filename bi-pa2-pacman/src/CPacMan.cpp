#include "CPacMan.hpp"

//----------------------------------------------------------------------------------------------------------------------
CPacMan::CPacMan ( CGameBoard         & board,
                   int                & PMDir,
                   CGameBoard::TCoord & PMPos,
                   int                & blDir,
                   CGameBoard::TCoord & blPos ) :
  CCharacter  ( board, PMDir, PMPos, blDir, blPos, NConfig::PMTMR, NConfig::PMTMR, NConfig::PMTMR )
{
  m_Pos = board . GetPacManStart ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CPacMan::ToFrght ( void )
{

}
//----------------------------------------------------------------------------------------------------------------------
void CPacMan::FromFrght ( void )
{

}
//----------------------------------------------------------------------------------------------------------------------
int CPacMan::MoveScttr ( void )
{
  return Move ( );
}
//----------------------------------------------------------------------------------------------------------------------
int CPacMan::MoveChase ( void )
{
  return Move ( );
}
//----------------------------------------------------------------------------------------------------------------------
int CPacMan::MoveFrght ( void )
{
  if ( ( m_Iter ++ ) % m_ScttrTmr )
    return 0;
  CGameBoard::TBoardTile * currTile = m_Board . GetCurrTile ( m_Pos ),
      * nextTile = m_Board . GetNextTile ( m_Pos, m_Dir ),
      * nextTeleport = nullptr;

  switch ( nextTile -> m_R )
  {
    case CGameBoard::ETileVal::FRESH:
      Step ( currTile, nextTile );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::DOT:
      m_Board . DotEaten ( nextTile -> m_Crd );
      Step ( currTile, nextTile );
      return NConfig::DOT_EATEN;
    case CGameBoard::ETileVal::BONUS:
      m_Board . BonusEaten ( nextTile -> m_Crd );
      Step ( currTile, nextTile );
      return NConfig::BONUS_EATEN;
    case CGameBoard::ETileVal::CHERRY:
      Step ( currTile, nextTile );
      return NConfig::CHERRY_EATEN;
    case CGameBoard::ETileVal::TELEPORT:
      currTile -> m_V = currTile -> m_R = CGameBoard::ETileVal::FRESH;
      nextTeleport = m_Board . GetNextTeleport ( nextTile -> m_Crd );
      m_Pos = nextTeleport -> m_Crd;
      nextTeleport -> m_R = CGameBoard::ETileVal::PACMAN;
      m_ChgTiles . emplace_back ( currTile );
      m_ChgTiles . emplace_back ( nextTeleport );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::BLINKY_FRGHT:
      Step ( currTile, nextTile );
      return NConfig::BLINKY_EATEN;
    case CGameBoard::ETileVal::PINKY_FRGHT:
      Step ( currTile, nextTile );
      return NConfig::PINKY_EATEN;
    case CGameBoard::ETileVal::INKY_FRGHT:
      Step ( currTile, nextTile );
      return NConfig::INKY_EATEN;
    case CGameBoard::ETileVal::CLYDE_FRGHT:
      Step ( currTile, nextTile );
      return NConfig::CLYDE_EATEN;
    default:
      break;
  }
  return NConfig::NO_CHANGE;
}
//----------------------------------------------------------------------------------------------------------------------
int CPacMan::Move ( void )
{
  if ( ( m_Iter ++ ) % m_ScttrTmr )
    return NConfig::NO_CHANGE;
  CGameBoard::TBoardTile * currTile = m_Board . GetCurrTile ( m_Pos ),
                         * nextTile = m_Board . GetNextTile ( m_Pos, m_Dir ),
                         * nextTeleport = nullptr;

  switch ( nextTile -> m_R )
  {
    case CGameBoard::ETileVal::FRESH:
      Step ( currTile, nextTile );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::DOT:
      m_Board . DotEaten ( nextTile -> m_Crd );
      Step ( currTile, nextTile );
      return NConfig::DOT_EATEN;
    case CGameBoard::ETileVal::BONUS:
      m_Board . BonusEaten ( nextTile -> m_Crd );
      Step ( currTile, nextTile );
      return NConfig::BONUS_EATEN;
    case CGameBoard::ETileVal::CHERRY:
      Step ( currTile, nextTile );
      return NConfig::CHERRY_EATEN;
    case CGameBoard::ETileVal::TELEPORT:
      currTile -> m_V = currTile -> m_R = CGameBoard::ETileVal::FRESH;
      nextTeleport = m_Board . GetNextTeleport ( nextTile -> m_Crd );
      m_Pos = nextTeleport -> m_Crd;
      nextTeleport -> m_R = CGameBoard::ETileVal::PACMAN;
      m_ChgTiles . emplace_back ( currTile );
      m_ChgTiles . emplace_back ( nextTeleport );
      return NConfig::NO_CHANGE;
    case CGameBoard::ETileVal::BLINKY:
    case CGameBoard::ETileVal::PINKY:
    case CGameBoard::ETileVal::INKY:
    case CGameBoard::ETileVal::CLYDE:
      Step ( currTile, nextTile );
      return NConfig::PACMAN_EATEN;
    default:
      break;
  }
  return NConfig::NO_CHANGE;
}
//----------------------------------------------------------------------------------------------------------------------
void CPacMan::Step ( CGameBoard::TBoardTile * currTile, CGameBoard::TBoardTile * nextTile, bool frght )
{
  ( void ) frght;
  if ( currTile -> m_V == CGameBoard::ETileVal::TELEPORT )
    currTile -> m_R = CGameBoard::ETileVal::TELEPORT;
  else
    currTile -> m_V = currTile -> m_R = CGameBoard::ETileVal::FRESH;
  nextTile -> m_R = CGameBoard::ETileVal::PACMAN;
  m_ChgTiles . emplace_back ( currTile );
  m_ChgTiles . emplace_back ( nextTile );
  m_Pos = nextTile -> m_Crd;
}
//----------------------------------------------------------------------------------------------------------------------
