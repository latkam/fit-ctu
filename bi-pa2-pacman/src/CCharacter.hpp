#pragma once

#include "NConfig.hpp"
#include "CGameBoard.hpp"

#include <stdexcept>

/**
 * @class CCharacter
 * @brief Represents one character in the game (PacMan and ghosts Blinky, Pinky, Inky and Clyde).
 */
class CCharacter
{
public:
                        CCharacter         ( CGameBoard         & board,
                                             int                & PMDir,
                                             CGameBoard::TCoord & PMPos,
                                             int                & blDir,
                                             CGameBoard::TCoord & blPos,
                                             int                  scttrTmr = 1,
                                             int                  chaseTmr = 1,
                                             int                  frghtTmr = 1 );
  virtual              ~CCharacter         ( void ) = default;
  
  /**
   * @return Reference to the character's direction.
   */
  int &                 GetDir             ( void );
  
  /**
   * @return Reference to the character's position.
   */
  CGameBoard::TCoord &  GetPos             ( void );

  /**
   * @brief Prepares character for performing in Frightened mode (changes its appearance and renders it).
   */
  virtual void          ToFrght            ( void ) = 0;

  /**
   * @brief Prepares character for performing in Scatter and Chase mode (changes its appearance and renders it).
   */
  virtual void          FromFrght          ( void ) = 0;

  /**
   * @brief Moves character in Scatter mode (Ghosts are moving towards the maze corners).
   * @return NConfig::PACMAN_EATEN =PacMan was eaten by the ghost.\n
   *         NConfig::NO_CHANGE    =Nothing special happened.\n
   *         NConfig::DOT_EATEN    =PacMan has eaten a dot.\n
   *         NConfig::BONUS_EATEN  =PacMan has eaten a bonus dot.\n
   *         NConfig::CHERRY_EATEN =PacMan has eaten cherries.\n
   *         NConfig::BLINKY_EATEN =PacMan has eaten Blinky (in Frightened mode).\n
   *         NConfig::PINKY_EATEN  =PacMan has eaten Pinky (in Frightened mode).\n
   *         NConfig::INKY_EATEN   =PacMan has eaten Inky (in Frightened mode).\n
   *         NConfig::CLYDE_EATEN  =PacMan has eaten Clyde (in Frightened mode).
   */
  virtual int           MoveScttr          ( void ) = 0;

  /**
   * @brief Moves character in Chase mode (Chosts are chasing PacMan).
   * @return See MoveScttr.
   */
  virtual int           MoveChase          ( void ) = 0;

  /**
   * @brief Moves character in Frightened mode (ghosts are slow and can be killed).
   * @return See MoveScttr.
   */
  virtual int           MoveFrght          ( void ) = 0;
  
  /**
   * @brief Sets the character's position.
   * @param[in] newDir New direction to be set.
   */
  void                  SetDir             ( int newDir );
  
  /**
   * @brief Turns the character back.
   */
  void                  TurnBack           ( void );

  /**
   * @brief Renders all (and only) tiles that have been changed in the last session.
   * @param[in, out] Win window to be rendered in.
   * @param[in] winW Window's width.
   * @param[in] winH Window's height.
   */
  void                  RenderChangedTiles ( WINDOW * win, int winW, int winH );


protected:

  /**
   * @brief Performs one step.
   * @param[in, out] currTile Current tile.
   * @param[in, out] nextTile Next tile.
   * @param[in] frght Determines if the game is in Frightened mode or not.
   */
  virtual void          Step               ( CGameBoard::TBoardTile * currTile, CGameBoard::TBoardTile * nextTile, bool frght = false ) = 0;
  
  /**
   * @brief Pinky, Inky and Clyde can leave their houses.
   * @param[in] tile Character's tile in non-Frightened mode. 
   * @param[in] tileFrght Character's tile in Frightened mode.
   * @param[in] frght Determines if the game is in Frightened mode or not. 
   */
  int                   LeaveHouse         ( CGameBoard::ETileVal tile, CGameBoard::ETileVal tileFrght, bool frght = false );

  std::vector<CGameBoard::TBoardTile *>           m_ChgTiles; /**< Vector of changed tiles to be rendered. */
  CGameBoard                                    & m_Board;    /**< Reference to the game board characters are moving at. */
  int                                             m_Dir;      /**< Character's direction. */
  CGameBoard::TCoord                              m_Pos;      /**< Character's position. */
  int                                           & m_PMDir;    /**< Reference to the actual PacMan's direction (needed for some heuristics computations). */
  CGameBoard::TCoord                            & m_PMPos;    /**< Reference to the actual PacMan's position (needed for some heuristics computations). */
  int                                           & m_BlDir;    /**< Reference to the actual Blinky's direction (needed for some heuristics computations). */
  CGameBoard::TCoord                            & m_BlPos;    /**< Reference to the actual Blinky's position (needed for some heuristics computations). */
  int                                             m_Iter;     /**< Number of iterations performed. */
  int                                             m_ScttrTmr; /**< Timer for Scatter mode. */
  int                                             m_ChaseTmr; /**< Timer for Chase mode. */
  int                                             m_FrghtTmr; /**< Timer for Frightened mode. */
  bool                                            m_InHouse;  /**< Indicates whether the character is still in the house. */
};
