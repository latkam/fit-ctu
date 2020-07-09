#pragma once

#include "NConfig.hpp"
#include "CGameBoard.hpp"
#include "TScore.hpp"

#include <map>
#include <functional>
#include <ncurses.h>

/**
 * @class CContext
 * @brief Superclass for subprograms. Stores context that subprograms are working with.
 * @note  Copy constructor and operator = are disabled.
 */
class CContext
{
public:
                 CContext   ( WINDOW * inWin, int inWinH, int inWinW );
                 CContext   ( const CContext & x ) = delete;
  virtual       ~CContext   ( void ) = default;
  CContext     & operator = ( const CContext & x ) = delete;
  
  /**
   * @brief Runs the subprogram.
   */
  virtual void   Run        ( void ) = 0;

protected:
  /**
   * @brief Clears any number of lines provided.
   * @param[in] lines Ncurses y-coordinates of all lines.
   */
  void          ClearInWinLine ( const std::vector<int> & lines );

  /**
   * @brief Makes the cursor and typed text visible.
   */
  static void   Visible        ( void );

  /**
   * @brief Makes the cursor and typed text invisible.
   */
  static void   Invisible      ( void );

  /**
   * @brief Waits for key input until the user presses ENTER.
   */
  void          WaitForEnter   ( void );

  /**
   * @brief Initialises inner window for the subprogram.
   * @param title Subprogram's title.
   */
  void          InitInWin      ( const char * title );

  /**
   * @brief Waits for user input, deletes content of the inner window and exits it.
   */
  void          ExitInWin      ( void );

  WINDOW                                   * m_InWin;  /**< Inner window. */
  int                                        m_InWinH; /**< Inner window height. */
  int                                        m_InWinW; /**< Inner window width. */
  static std::map<std::string, CGameBoard>   s_Boards; /**< Loaded game boards. */
  static std::vector<TScore>                 s_Scores; /**< Database of player's scores. */
};
