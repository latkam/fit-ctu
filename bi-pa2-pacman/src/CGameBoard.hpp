#pragma once

#include "NConfig.hpp"

#include <cmath>
#include <cstring>
#include <climits>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <ncurses.h>

/**
 * @class CGameBoard
 * @brief Represents game board.
 */
class CGameBoard
{
public:

  /**
   * @struct TCoord
   * @brief Represents set of coordinates.
   */
  struct TCoord
  {
    int m_X; /**< X-coordinate. */
    int m_Y; /**< Y-coordinate. */

                  TCoord ( int x = 0, int y = 0 );
    bool operator ==     ( const TCoord & x ) const;
    bool operator !=     ( const TCoord & x ) const;
  };

  /**
   * @enum ETileVal
   * @brief represents possible tile values
   */
  enum class ETileVal
  {
    WALL_CORNER     = '+',
    WALL_HORIZONTAL = '-',
    WALL_VERTICAL   = '|',
    DOOR            = '=',
    PACMAN          = 'C',
    BLINKY_FRGHT    = 'B', /**< ETileVal::BLINKY in CGamePlayer::EGameMode::FRIGHTENED. */
    PINKY_FRGHT     = 'P', /**< ETileVal::PINKY  in CGamePlayer::EGameMode::FRIGHTENED. */
    INKY_FRGHT      = 'I', /**< ETileVal::INKY   in CGamePlayer::EGameMode::FRIGHTENED. */
    CLYDE_FRGHT     = 'L', /**< ETileVal::CLYDE  in CGamePlayer::EGameMode::FRIGHTENED. */
    BLINKY          = 'b',
    PINKY           = 'p',
    INKY            = 'i',
    CLYDE           = 'c',
    FRESH           = ' ',
    DOT             = '.',
    BONUS           = 'o',
    CHERRY          = '%',
    TELEPORT        = 'T'
  };

  /**
   * @struct TBoardTile
   * @brief Represents one tile.
   */
  struct TBoardTile
  {
    TCoord   m_Crd; /**< Coordinates. */
    ETileVal m_V;   /**< Node value. */
    ETileVal m_R;   /**< Node value to render. */

                TBoardTile ( int x = 0, int y = 0, char value = ' ' );
    void        Render     ( WINDOW * win, int y, int x, bool small = false ) const;
  };

                        CGameBoard      ( void );

  /**
   * @brief Loads board data from the file and check's whether the format is ok.
   * @param[in] fileName
   * @param[in, out] win Window to display user dialogue and loaded board.
   * @return true=board loaded successfully, false=board loading failed
   */
  bool                  LoadBoardData   ( const char * fileName, WINDOW * win );

  /**
   * @brief Sets game modes and ghosts timeouts according to user-defined game difficulty and ghost difficulty.
   */
  void                  SetGameAttr     ( void );

  /**
   * @return Number of elements for the PacMan to eat.
   */
  size_t                GetNmbToEat     ( void ) const;
  
  /**
   * @return Reference to PacMan's start position.
   */
  TCoord              & GetPacManStart  ( void );

  /**
   * @return Starting positions for all ghosts.
   */
  std::vector<TCoord> & GetGhStart      ( void );

  /**
   * @return CGamePlayer::EGameMode::SCATTER mode timer.
   */
  int                   GetScttrTmr     ( void ) const;

  /**
   * @return CGamePlayer::EGameMode::CHASE mode timer.
   */
  int                   GetChaseTmr     ( void ) const;

  /**
   * @return CGamePlayer::EGameMode::FRIGHTENED mode timer.
   */
  int                   GetFrghtTmr     ( void ) const;

  /**
   * @return Ghost timer for CGamePlayer::EGameMode::SCATTER mode.
   */
  int                   GetGhScttrTmr   ( void ) const;

  /**
   * @return Ghost timer for CGamePlayer::EGameMode::CHASE mode.
   */
  int                   GetGhChaseTmr   ( void ) const;
  
  /**
   * @return Maximal x-coordinate of the game board.
   */
  int                   GetMaxX         ( void ) const;
  
  /**
   * @return Maximal y-coordinate of the game board.
   */
  int                   GetMaxY         ( void ) const;

  /**
   * @brief Computes Euclidean distance between two tiles.
   * @param[in] a First tile.
   * @param[in] b Second tile.
   * @return Computed distance.
   */
  double                EclDist         ( const TCoord & a, const TCoord & b ) const;
  
  /**
   * @brief Renders the game board.
   */
  void                  Render          ( WINDOW * win, int offsetX = 0, int offsetY = 0 ) const;

  /**
   * @brief Resets the game board to the initial state like before any game.
   */
  void                  CleanForNewGame ( bool eaten );

  /**
   * @brief Adds cherry to the game board and renders it.
   */
  void                  SetCherry       ( WINDOW * win, int offsetX = 0, int offsetY = 0 );
  
  /**
   * @param[in] Current position.
   * @return Pointer to the current tile according to the passed position.
   */
  TBoardTile          * GetCurrTile     ( const TCoord & position );
  
  /**
   * @brief Returns pointer to the next tile according to the character's direction.
   * @throws std::invalid_argument if direction is not in <0;3> interval
   */
  TBoardTile          * GetNextTile     ( const TCoord & position, int direction );

  /**
   * @brief Gets pointer to the next tile that is the best according to the Euclidean distance from the target.
   * Used to determine the best direction when a ghost is at the crossing.
   * @details The best tile cannot be behind the ghost. If it is, the second best tile is returned.
   */
  TBoardTile          * GetBestNextTile ( const TCoord & position, int & direction, const TCoord & target );

  /**
   * @brief Gets pointer to a tile in the specified distance defined by direction, x-coordinate and y-coordinate differences.
   * @param[in] position Actual position.
   * @param[in] direction Actual direction.
   * @param[in] diffX x-coordinate difference.
   * @param[in] diffY y-coordinate difference.
   * @throws std::invalid_argument if direction is not in the <0;3> interval.
   */
  TBoardTile          * GetTileDistance ( const TCoord & position, int direction, int diffX, int diffY, bool regardDirection = true );
  
  /**
   * @brief Checks if the position is equivalent to any stored teleport and if so, returns pointer to the next stored teleport.
   * @throws std::logic_error if number of teleports is less than 2. See NConfig::TELEPORTS.
   */
  TBoardTile          * GetNextTeleport ( const TCoord & position );
  
  /**
   * @brief Checks if the tile is crossing or not.
   * @details Crossings:
   * \verbatim
        x   x   x
       xxx xxx xx 
        x
     \endverbatim
   * and all rotations.
   */
  bool                  IsCrossing      ( const TCoord & position, int direction );

  /**
   * @brief Marks one ETileVal::DOT eaten.
   */
  void                  DotEaten        ( const TCoord & position );

  /**
   * @brief Marks one ETileVal::BONUS eaten.
   */
  void                  BonusEaten      ( const TCoord & position );

private:
  /**
   * @enum EDifficulty
   * @brief Represents difficulty levels.
   */
  enum class EDifficulty
  {
    EASY   = 0,
    MEDIUM = 1,
    HARD   = 2
  };

  /**
   * @brief Displays an error message while loading maze.
   */
  static bool                          ErrorMessage        ( WINDOW * win, std::ifstream & in, const char * message );

  /**
   * @brief Stores initial positions for PacMan, ghosts and some other kinds of tiles.
   * @return true=appropriate number of characters is stored, they are in the right order
   *              and other tiles are set appropriately
   *         false=otherwise
   */
  bool                                 StorePosInfo        ( CGameBoard::TBoardTile & tile );

  /**
   * @brief Checks whether board edges are correct.
   * @details Corners must consist of ETileVal::WALL_CORNER, vertical walls of ETileVal::WALL_VERTICAL and horizontal walls of ETileVal::WALL_HORIZONTAL.
   */
  bool                                 CheckBoardIntegrity ( void );

  /**
  * @brief Checks whether the ghost home is set up properly. Over each ghost, there has to be a tile called DOOR, so that
  * the gost can get out from its home.
  * @note Only Pinky, Inky and Clyde need home, so Blinky is excluded as his starting position is somewhere on the maze
  * corridor.
  * @return true=ok
  *         false = fail
  */
  bool                                 CheckGhostHome      ( const TCoord & position );

  /**
   * @brief Check if no ghost start position has already been set (aka is equal to (0, 0) ).
   */
  bool                                 GhStartEmpty        ( void );


  std::vector<std::vector<TBoardTile>> m_Tiles;        /**< Board tiles. First coordinate is y, second coordinate is x. */
  int                                  m_MaxX;         /**< Max x-coordinate. */
  int                                  m_MaxY;         /**< Max y-coordinate. */
  std::vector<TCoord>                  m_Teleports;    /**< Vector of teleports. Their number is set in NConfig. */
  std::vector<TCoord>                  m_Freshes;      /**< Vector of ETileVal::FRESH tiles coordinates. */
  std::vector<TCoord>                  m_Dots;         /**< Vector of ETileVal::DOT tiles coordinates. */
  std::vector<TCoord>                  m_Bonuses;      /**< Vector of ETileVal::BONUS tiles coordinates. */
  std::vector<TCoord>                  m_DotsEaten;    /**< Vector of ETileVal::DOT tiles eaten. */
  std::vector<TCoord>                  m_BonusesEaten; /**< Vector of ETileVal::BONUS tiles eaten. */
  EDifficulty                          m_GameDiff;     /**< User-defined game difficulty.*/
  EDifficulty                          m_GhostDiff;    /**< User-defined ghost difficulty. */
  TCoord                               m_PacManStart;  /**< PacMan's starting position. */
  std::vector<TCoord>                  m_GhostStart;   /**< Ghost's starting positions. */
  TCoord                               m_CherryPos;    /**< Position of the ETileVal::CHERRY tile. */
  bool                                 m_PacManSet;    /**< Indicates whether user has already defined PacMan's starting position. */
  std::set<ETileVal>                   m_GhostSet;     /**< Indicates which ghosts user has already defined. */
  bool                                 m_CherrySet;    /**< Indicates whether user has already defined cherry's position. */
  double                               m_ScttrTmr;     /**< Timer for the CGamePlayer::EGameMode::SCATTER. */
  double                               m_ChaseTmr;     /**< Timer for the CGamePlayer::EGameMode::CHASE. */
  double                               m_FrghtTmr;     /**< Timer for the CGamePlayer::EGameMode::FRIGHTENED. */
  int                                  m_GhScttrTmr;   /**< Ghosts timer for the CGamePlayer::EGameMode::SCATTER. */
  int                                  m_GhChaseTmr;   /**< Ghosts timer for the CGamePlayer::EGameMode::CHASE. */
};
