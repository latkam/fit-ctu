#pragma once

#include "NConfig.hpp"
#include "CContext.hpp"
#include "CGamePlayer.hpp"
#include "CMazeLoader.hpp"
#include "CScoreShower.hpp"

#include <cstring>
#include <csignal>
#include <vector>
#include <memory>
#include <algorithm>
#include <ncurses.h>

/**
 * \mainpage
 * Implementation of classic arcade game PacMan as a semestral project for BI-PA2 (Programming and algorithmics 2)
 * course taught at FIT CTU in Prague.
 *
 * This game consists of three main modules as you can see in the main menu (see CGameManager):\n
 * PLAY GAME: user chooses one of stored mazes and plays the game (see CGamePlayer),\n
 * LOAD MAZE: user chooses a name for a maze and types a file name with the intended maze (see CMazeLoader),\n
 * SHOW SCORES: shows a table with scores. User can scroll up and down to see all scores in all mazes (see CScoreShower).
 *
 * Enjoy!
 *
 * Matěj Latka, June 2020.
 */

/**
 * @class CGameManager
 * @brief Launches the application and manages all subprograms.
 * @note  Copy constructor and operator = are disabled.
 */
class CGameManager
{
public:
                 CGameManager ( void );
                 CGameManager ( const CGameManager & x ) = delete;
  CGameManager & operator =   ( const CGameManager & x ) = delete;

  /**
   * @brief Runs the entire application.
   */
  void           Run          ( void );

private:
  /**
   * @brief Runs application's main menu.
   */
  void           MainMenu     ( void );
  
  /**
   * @brief Checks if the terminal supports colors.
   *        If it does, the method defines all necessary colors.
   *        Otherwise black-and-white scheme is prepared.
   */
  static void    DefineColors ( void );
  
  int                                        m_OutWinH; /**< Outer window height. */
  int                                        m_OutWinW; /**< Outer window width. */
  WINDOW                                   * m_InWin;   /**< Inner window. */
  int                                        m_InWinH;  /**< Inner window height. */
  int                                        m_InWinW;  /**< Inner window width. */
  std::vector<std::unique_ptr<CContext>>     m_Subprgs; /**< Subprograms. */
  std::vector<std::string>                   m_MMText;  /**< Text displayed in main menu window. */
  size_t                                     m_MMH;     /**< Main menu window height. */
  size_t                                     m_MMW;     /**< Main menu window width. */
};
