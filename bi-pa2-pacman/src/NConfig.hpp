#pragma once

#include <string>
#include <set>
/**
 * @namespace NConfig
 * @brief Stores global constants used in application.
 */
namespace NConfig
{
  static const constexpr int      DIRECTIONS             = 4;
  
  /**
   * @brief Computes indent from the edge of the screen.
   * @param[in] area size of the dimension of the window
   * @param[in] size size of the element we want to get indent for
   * @return computed indent
   */
  int Indent ( const int area, const int size );

  /**
   * @brief Makes sure modulo output is non-negative and so thr result keeps in bounds.
   * @param[in] nmb number the modulo will be performed on
   * @param[in] mod modulo
   * @return number after modulo with non-negative output
   */
  int Mod ( int nmb, int mod = NConfig::DIRECTIONS );

  static const constexpr int      KEY_ENTER_BIG          = 10;   /**< Numeric value of the big enter key. */
  static const constexpr double   INWINSIZE              = 0.9;  /**< Ratio of the screen occupied by the inner window. */
  static const constexpr double   INWININDENT            = ( 1 - INWINSIZE ) / 2; /* Ratio for computation of the inner window height and width. */
  static const constexpr int      INPUT_MAX_LEN          = 50;  /**< Maximal length of user input. */
  static const constexpr int      SLEEP_TIME             = 50;  /**< Sleep time in milliseconds. */
  static const constexpr int      INWINSHIFTW            = 1;   /**< Horizontal render shift. */
  static const constexpr int      INWINSHIFTH            = 4;   /**< Vertical render shift. */
  static const constexpr int      TELEPORTS              = 2;   /**< Number of teleports. @warning If the value is less than 2, the CGameBoard::GetNextTeleport method throws std::logic_error. */
  static const constexpr int      LIVES                  = 3;   /**< Number of PacMans available in one game. */
  static const constexpr int      DIR_DEFAULT            = 3;   /**< Initial PacMan's direction. */
  static const constexpr int      DIR_LEAVE              = 0;   /**< Direction to leave the house. @warning Should not be changed as CCharacter::LeaveHouse and CGameBoard::CheckGhostHome expects 0 (the ghost will go upwards to leave the house).*/
  static const constexpr int      HOME_MIN_Y             = 3;   /**< Minimum y-coordinate of ghost home (how much space the ghost needs to leave the house). @warning Should not be less than two as leaving the house upwards won't be possible. */
  static const constexpr int      PINKY_TARGET_DIST      = 4;   /**< Distance between PacMan's position and Pinky's target tile. */
  static const constexpr int      INKY_TARGET_DIST       = 2;   /**< Distance between PacMan's position and the tile that will be used to compute Inky's vector. */
  static const constexpr double   CLYDE_TARGET_DIST      = 5;   /**< Distance between PacMan's and Clyde's position that separates two modes of Clyde's chase behaviour. */
  static const constexpr int      INKY_DELAY             = 50;  /**< Delay between the start of the game and Inky's leave time. */
  static const constexpr int      CLYDE_DELAY            = 100; /**< Delay between the start of the game and Clyde's leave time. */
  static const constexpr int      NMB_GHOSTS             = 4;   /**< Number of ghosts in the game. @warning Should not be changed as the game and its rules are designed for 4 ghosts. */
  static const constexpr int      PMTMR                  = 3;   /**< PacMan timer (in all modes). */
  static const constexpr int      GHFRGHTTMR             = 4;   /**< Ghost timer in Frightened mode. */
  static const constexpr int      SCORES_SHOW_FIRST_LINE = 4;   /**< Position of the first line where the scores will be shown. */
  static const constexpr int      SCORES_SHOW_LEFT_INDT  = 2;   /**< Left indent for showing scores. */

  /* Return values of Move methods. */
  static const constexpr int      NO_CHANGE              = 0;   /**< Nothing special happened. */
  static const constexpr int      PACMAN_EATEN           = -1;  /**< PacMan was eaten by the ghost in non-Frightened mode. */
  static const constexpr int      DOT_EATEN              = 10;  /**< PacMan has eaten a CGameBoard::ETileVal::DOT. */
  static const constexpr int      BONUS_EATEN            = 50;  /**< PacMan has eaten a CGameBoard::ETileVal::BONUS. */
  static const constexpr int      CHERRY_EATEN           = 100; /**< PacMan has eaten a CGameBoard::ETileVal::CHERRY. */
  static const constexpr int      GHOST_EATEN            = 200; /**< PacMan has eaten a CGameBoard::ETileVal::CHERRY. */
  static const constexpr int      BLINKY_EATEN           = 200; /**< PacMan has eaten Blinky in non-Frightened mode. */
  static const constexpr int      PINKY_EATEN            = 300; /**< PacMan has eaten Pinky in non-Frightened mode. */
  static const constexpr int      INKY_EATEN             = 400; /**< PacMan has eaten Inky in non-Frightened mode. */
  static const constexpr int      CLYDE_EATEN            = 500; /**< PacMan has eaten Clyde in non-Frightened mode. */

  /* User dialogue lines. */
  static const constexpr int      GP_DIALOGUE_Q_LINE     = 2;   /**< CGamePlayer dialogue question line. */
  static const constexpr int      GP_DIALOGUE_S_LINE     = 3;   /**< CGamePlayer dialogue suggestions line. */
  static const constexpr int      GP_DIALOGUE_I_LINE     = 4;   /**< CGamePlayer dialogue input line. */
  static const constexpr int      GP_DIALOGUE_R_LINE     = 5;   /**< CGamePlayer dialogue result line. */
  static const constexpr int      ML_DIALOGUE_Q_LINE     = 2;   /**< CMazeLoader dialogue question line. */
  static const constexpr int      ML_DIALOGUE_I_LINE     = 3;   /**< CMazeLoader dialogue input line. */
  static const constexpr int      ML_DIALOGUE_R_LINE     = 4;   /**< CMazeLoader dialogue result line. */

  /* Directions. */
  static const constexpr int      DIR_UP                 = 0;
  static const constexpr int      DIR_RIGHT              = 1;
  static const constexpr int      DIR_DOWN               = 2;
  static const constexpr int      DIR_LEFT               = 3;

  static const constexpr char     STOP_INPUT_CHAR        = 'g'; /**< Character stopping a loop loading maze in CGameBoard::LoadBoardData (first character of game difficulty. */
  static const std::string        INPUT_GAME             = "ame";
  static const std::string        INPUT_GHOST            = "ghost";
  static const std::string        INPUT_DIFFICULTY       = "difficulty";
  static const std::string        DIFFICULTY_EASY        = "easy";
  static const std::string        DIFFICULTY_MEDIUM      = "medium";
  static const std::string        DIFFICULTY_HARD        = "hard";
  static const std::string        EXCP_INVALID_DIR       = "Invalid direction.";
  static const std::string        EXCP_UNKNOWN_TELEPORT  = "Unknown teleport position.";
  static const std::string        EXCP_FEW_TELEPORTS     = "Too few teleports (need at least 2).";
  static const std::string        FOR_EXAMPLE            = "For example ";
  static const std::string        DEFAULT_DIR            = "./examples"; /**< Default directory to search for files with mazes. */
  static const constexpr char   * HEAD_PACMAN            = "PACMAN";
  static const constexpr char   * HEAD_MAIN_MENU         = "MAIN MENU";
  static const constexpr char   * HEAD_PLAY_GAME         = "PLAY GAME";
  static const constexpr char   * HEAD_LOAD_MAZE         = "LOAD MAZE";
  static const constexpr char   * HEAD_SHOW_SCORE        = "SHOW SCORES";
  static const constexpr char   * EXIT                   = "EXIT";
  static const constexpr char   * PLEASE_ENTER_MAZE_NAME = "Please choose a name for your maze (max. %d characters): ";
  static const constexpr char   * PLEASE_ENTER_MAZE_PATH = "Please enter file name with the maze (mazeXXXX.in, where XXXX is a number): ";
  static const constexpr char   * NO_MAZE_AVAILABLE      = "There are no mazes available, before playing, please load one.";
  static const constexpr char   * ALREADY_MAZE           = "Maze with this name already exists. Please enter a different name.";
  static const constexpr char   * NO_SUCH_MAZE           = "Maze with this name does not exist. Please enter a different name.";
  static const constexpr char   * INPUT_MAZE_SUCCESS     = "Maze loaded successfully. Press any key to display it.";
  static const constexpr char   * INPUT_MAZE_FAIL        = "Maze loading failed.";
  static const constexpr char   * INPUT_GAME_DIFF_FAIL   = "Loading game difficulty failed.";
  static const constexpr char   * INPUT_GHOST_DIFF_FAIL  = "Loading ghost difficulty failed.";
  static const constexpr char   * ENTER_START            = "Press ENTER to start game.";
  static const constexpr char   * ENTER_RESUME           = "Press ENTER to resume game.";
  static const constexpr char   * ENTER_END              = "Press ENTER to end game.";
  static const constexpr char   * WON                    = "YOU WON!";
  static const constexpr char   * LOST                   = "YOU LOST!";
  static const constexpr char   * SCORES_DELIMITER       = "+----------------------------------------------------+---------+-----+";
  static const constexpr char   * SCORES_COLUMNS         = "|                     MAZE NAME                      |  SCORE  | WON |";
  static const constexpr char   * NO_SCORES              = "|                Nothing to show...                                  |";
  static const constexpr char   * SCORES_LINE            = "| %50s | %7d | %3s |"; /**< One line in CScoreShower::ShowScores. */
  static const constexpr char   * ANY_KEY_EXIT           = "Press any key to exit the window.";
  static const constexpr char   * ERROR_CANNOT_OPEN      = "Unable to open file.";
  static const constexpr char   * ERROR_CANNOT_READ      = "Cannot read from the file.";
  static const constexpr char   * ERROR_INVALID_TILE     = "Invalid tile.";
  static const constexpr char   * ERROR_PACMAN_SETUP     = "Pac-Man, Ghosts or Cherry are not set up properly.";
  static const constexpr char   * ERROR_INVALID_TELEPORT = "Invalid number of teleports.";
  static const constexpr char   * ERROR_INTEGRITY        = "Board integrity check or ghost home check failed.";
  static const std::set<char>     POSS_TILES             = { '+', '-', '|', '.', ' ', 'o', 'C', 'T', '=', 'b', 'p', 'i', 'c', '%', '\n' }; /**< Possible tiles in the maze. */
}
