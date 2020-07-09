#pragma once

#include <string>

/**
 * @struct TScore.
 * @brief Stores score info.
 */
class TScore
{
public:
  std::string m_BoardName; /**< Name of the board. */
  int         m_Score;     /**< Score. */
  std::string m_Won;       /**< Indicator, whether the player has won. */

  TScore ( const std::string & boardName = "N/A", int score = 0, const std::string & won = "NO" );
};
