#include "CCharacter.hpp"

//----------------------------------------------------------------------------------------------------------------------
CCharacter::CCharacter ( CGameBoard         & board,
                         int                & PMDir,
                         CGameBoard::TCoord & PMPos,
                         int                & blDir,
                         CGameBoard::TCoord & blPos,
                         int                  scttrTmr,
                         int                  chaseTmr,
                         int                  frghtTmr ) :
    m_Board    ( board ),
    m_Dir      ( NConfig::DIR_DEFAULT ),
    m_Pos      ( CGameBoard::TCoord ( 0, 0 ) ),
    m_PMDir    ( PMDir ),
    m_PMPos    ( PMPos ),
    m_BlDir    ( blDir ),
    m_BlPos    ( blPos ),
    m_Iter     ( 0 ),
    m_ScttrTmr ( scttrTmr ),
    m_ChaseTmr ( chaseTmr ),
    m_FrghtTmr ( frghtTmr ),
    m_InHouse  ( true ) { }
//----------------------------------------------------------------------------------------------------------------------
void CCharacter::SetDir ( int newDir )
{
  CGameBoard::TBoardTile * nextTile = m_Board . GetNextTile ( m_Pos, newDir );
  if ( nextTile -> m_V == CGameBoard::ETileVal::WALL_CORNER
    || nextTile -> m_V == CGameBoard::ETileVal::WALL_HORIZONTAL
    || nextTile -> m_V == CGameBoard::ETileVal::WALL_VERTICAL )
    return;
  m_Dir = NConfig::Mod ( newDir );
}
//----------------------------------------------------------------------------------------------------------------------
int & CCharacter::GetDir ( void )
{
  return m_Dir;
}
//----------------------------------------------------------------------------------------------------------------------
CGameBoard::TCoord & CCharacter::GetPos ( void )
{
  return m_Pos;
}
//----------------------------------------------------------------------------------------------------------------------
void CCharacter::TurnBack ( void )
{
  SetDir ( NConfig::Mod ( m_Dir + 2 ) );
}
//----------------------------------------------------------------------------------------------------------------------
void CCharacter::RenderChangedTiles ( WINDOW * win, int winW, int winH )
{
  for ( const auto tile : m_ChgTiles )
    tile -> Render ( win,
                     NConfig::Indent ( winH, m_Board . GetMaxY ( ) ) + NConfig::INWINSHIFTH + tile -> m_Crd . m_Y,
                     NConfig::Indent ( winW, m_Board . GetMaxX ( ) ) + NConfig::INWINSHIFTW + tile -> m_Crd . m_X );
  wrefresh ( win );
  m_ChgTiles . clear ( );
}
//----------------------------------------------------------------------------------------------------------------------
int CCharacter::LeaveHouse ( CGameBoard::ETileVal tile, CGameBoard::ETileVal tileFrght, bool frght )
{
  CGameBoard::TBoardTile * houseTile = m_Board . GetCurrTile ( m_Pos ), * doorTile, * outTile = nullptr;
  doorTile = m_Board . GetNextTile ( m_Pos, NConfig::DIR_LEAVE );
  outTile = m_Board . GetNextTile ( doorTile -> m_Crd, NConfig::DIR_LEAVE );

  houseTile -> m_V = houseTile -> m_R = CGameBoard::ETileVal::FRESH;
  m_ChgTiles . emplace_back ( houseTile );
  m_Pos = outTile -> m_Crd;
  frght ? outTile -> m_R = tileFrght : outTile -> m_R = tile;
  m_ChgTiles . emplace_back ( outTile );
  m_InHouse = false;

  if ( outTile -> m_R == CGameBoard::ETileVal::PACMAN )
  {
    if ( frght )
    {
      switch ( tileFrght )
      {
        case CGameBoard::ETileVal::BLINKY_FRGHT:
          return NConfig::BLINKY_EATEN;
        case CGameBoard::ETileVal::PINKY_FRGHT:
          return NConfig::PINKY_EATEN;  
        case CGameBoard::ETileVal::INKY_FRGHT:
          return NConfig::INKY_EATEN;
        case CGameBoard::ETileVal::CLYDE_FRGHT:
          return NConfig::CLYDE_EATEN;
        default:
          break;      
      }
    }
    return NConfig::PACMAN_EATEN;
  }
  return NConfig::NO_CHANGE;
}
//----------------------------------------------------------------------------------------------------------------------
