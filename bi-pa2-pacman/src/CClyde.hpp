#pragma once

#include "CCharacter.hpp"

/**
 * @class CClyde
 * @brief Represents Clyde (orange ghost).
 */
class CClyde : public CCharacter
{
public:
               CClyde     ( CGameBoard         & board,
                            int                & PMDir,
                            CGameBoard::TCoord & PMPos,
                            int                & blDir,
                            CGameBoard::TCoord & blPos );
  virtual void ToFrght    ( void ) override;
  virtual void FromFrght  ( void ) override;
  virtual int  MoveScttr  ( void ) override;
  virtual int  MoveChase  ( void ) override;
  virtual int  MoveFrght  ( void ) override;
  
private:
  virtual void Step       ( CGameBoard::TBoardTile * currTile, CGameBoard::TBoardTile * nextTile, bool frght = false ) override;

  int                m_LHRet;       /**< Return value of the CCharacter::LeaveHouse. */
  CGameBoard::TCoord m_ScttrCorner; /**< Target corner in CGamePlayer::EGameMode::SCATTER mode. */
};
