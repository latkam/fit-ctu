#pragma once

#include "CContext.hpp"

/**
 * @class CScoreShower
 * @brief Shows scores ordered from the best to the worst.
 */
class CScoreShower : public CContext
{
public:
               CScoreShower ( WINDOW * inWin, int inWinH, int inWinW );
  virtual void Run          ( void ) override;

private:

  /**
   * @brief Sorts scores descending by points and victory, respectively.
   */
  static void  SortScores   ( void );
  void         ShowScores   ( void );
};
