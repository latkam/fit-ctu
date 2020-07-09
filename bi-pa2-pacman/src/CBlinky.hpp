#pragma once

#include "CCharacter.hpp"

/**
 * @class CBlinky
 * @brief Represents Blinky (red ghost)
 */
class CBlinky : public CCharacter
{
public:
               CBlinky   ( CGameBoard         & board,
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
  virtual void Step      ( CGameBoard::TBoardTile * currTile, CGameBoard::TBoardTile * nextTile, bool frght = false ) override;
};
