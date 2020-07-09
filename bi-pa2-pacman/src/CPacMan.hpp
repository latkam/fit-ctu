#pragma once

#include "CCharacter.hpp"
#include "CGameBoard.hpp"

/**
 * @class CPacMan
 * @brief Represents PacMan.
 */
class CPacMan : public CCharacter
{
public:
               CPacMan   ( CGameBoard         & board,
                           int                & PMDir,
                           CGameBoard::TCoord & PMPos,
                           int                & blDir,
                           CGameBoard::TCoord & blPos );
  virtual void ToFrght   ( void ) override;
  virtual void FromFrght ( void ) override;
  virtual int  MoveScttr ( void ) override;
  virtual int  MoveChase ( void ) override;
  virtual int  MoveFrght ( void ) override;

private:

  /**
   * @brief Move in all game modes as PacMan is not affected by them.
   */
  int           Move      ( void );
  virtual void  Step      ( CGameBoard::TBoardTile * currTile, CGameBoard::TBoardTile * nextTile, bool frght = false ) override;
};
