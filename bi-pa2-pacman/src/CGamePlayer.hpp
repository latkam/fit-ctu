#pragma once

#include "NConfig.hpp"
#include "CContext.hpp"
#include "CCharacter.hpp"
#include "CPacMan.hpp"
#include "CBlinky.hpp"
#include "CPinky.hpp"
#include "CInky.hpp"
#include "CClyde.hpp"

#include <chrono>
#include <thread>

/**
 * @class CGamePlayer
 * @brief Plays the game.
 */
class CGamePlayer : public CContext
{
public:
               CGamePlayer            ( WINDOW * inWIn, int inWinH, int inWinW );
  virtual void Run                    ( void ) override;

private:

  /**
   * @brief Prepares the game - manages user dialogue to get the maze the game will be played in.
   * @return true=maze has been set up properly and the game will be played.\n
   *         false=maze has not been set up properly and the game cannot be played.
   */
  bool         PrepareGame            ( void );
  
  /**
   * @brief Suggests user the first maze name that can be used.
   */
  void         SuggestMaze            ( void );
  
  /**
   * @brief Cleans the vector with characters and constructs new with default attributes and position.
   */
  void         ResetCharacters        ( void );
  
  /**
   * @brief Manages the basic game loop and the user input. 
   */
  void         PlayGame               ( void );
  
  /**
   *  @brief Changes the m_GameMode mode after defined period.
   *  @details EGameMode::SCATTER -> EGameMode::CHASE -> EGameMode::SCATTER...\n
   *           EGameMode::{SCATTER|CHASE} -(PacMan eats CGameBoard::ETileVal::DOT)-> EGameMode::FRIGHTENED -> EGameMode::SCATTER...
   */
  void         UpdateGameMode         ( void );
  
  /**
   * @brief Switches ghosts from normal to frightened appearance.
   */
  void         CharactersToFrght      ( void );
  
  /**
   * @brief Switches ghosts from frightened to normal appearance.
   */
  void         CharactersFromFrght    ( void );
  
  /**
   * @brief Moves all characters according to m_GameMode.
   * @return true=the game can continue (UpdateDynamicInfo returned true)\n
   *         false=the game cannot continue (UpdateDynamicInfo returned false)
   *          
   */
  bool         MoveAll                ( void );
  
  /**
   * @brief Turns all ghosts when changing the game mode.
   */
  void         TurnGhosts             ( void );
  
  /**
   * @brief Renders dynamic game info (score and remaining PacMans).
   */
  void         RenderDynamicInfo      ( void );
  
  /**
   * @brief Updates dynamic game info (score and remaining PacMans)
            according to the return value of CCharacterMoveScttr, CCharacterMoveChase or CCharacterMoveFrght.
   * @param[in] retVal 
   * @return true=the game can continue (there is at least one remaining PacMan)\n
   *         false=the game cannot continue (there is no remaining PacMan)
   */
  bool         UpdateDynamicInfo      ( int retVal );
  
  /**
   * @brief Shows the game result (the player has won or the player has lost) and stores it to s_Scores.
   */
  void         ShowAndStoreGameResult ( void );

  enum class EGameMode
  {
    SCATTER    = 0,
    CHASE      = 1,
    FRIGHTENED = 2
  };

  char                                     m_BrdName[NConfig::INPUT_MAX_LEN + 1]; /**< Board name. */
  int                                      m_BrdIndW;                             /**< Board indent x-axis. */
  int                                      m_BrdIndH;                             /**< Board indent y-axis. */
  int                                      m_PMRem;                               /**< Number of PacMans remaining. */
  size_t                                   m_ToEat;                               /**< Number of elements to be eaten. */
  int                                      m_Iter;                                /**< Current iteration. */
  EGameMode                                m_GameMode;                            /**< Current game mode. */
  int                                      m_Score;                               /**< Current score. */
  int                                      m_GhEaten;                             /**< Number of ghosts eaten by PacMan. */
  std::vector<std::unique_ptr<CCharacter>> m_Characters;                          /**< Characters involved in the game. */
};
