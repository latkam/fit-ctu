#include "CScoreShower.hpp"

//----------------------------------------------------------------------------------------------------------------------
CScoreShower::CScoreShower ( WINDOW * inWin, int inWinH, int inWinW ) :
  CContext ( inWin, inWinH, inWinW ) { }
//----------------------------------------------------------------------------------------------------------------------
void CScoreShower::Run ( void )
{
  InitInWin  ( NConfig::HEAD_SHOW_SCORE );
  SortScores ( );
  ShowScores ( );
  ExitInWin  ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CScoreShower::SortScores ( void )
{
  std::sort ( s_Scores . begin ( ),
              s_Scores . end   ( ),
              [] ( const TScore & a, const TScore & b )
                 { if ( a . m_Score != b . m_Score )
                     return a . m_Score > b . m_Score;
                   return a . m_Won > b . m_Won; } );
}
//----------------------------------------------------------------------------------------------------------------------
void CScoreShower::ShowScores ( void )
{
  int headerIndent = 0, contentIndent = 0, input = KEY_UP;
  size_t range, shift = 0;
  mvwprintw ( m_InWin,
              NConfig::SCORES_SHOW_FIRST_LINE + headerIndent ++,
              NConfig::SCORES_SHOW_LEFT_INDT,
              NConfig::SCORES_DELIMITER );
  mvwprintw ( m_InWin,
              NConfig::SCORES_SHOW_FIRST_LINE + headerIndent ++,
              NConfig::SCORES_SHOW_LEFT_INDT,
              NConfig::SCORES_COLUMNS );
  mvwprintw ( m_InWin,
              NConfig::SCORES_SHOW_FIRST_LINE + headerIndent ++,
              NConfig::SCORES_SHOW_LEFT_INDT,
              NConfig::SCORES_DELIMITER );

  if ( s_Scores . empty ( ) )
  {
    mvwprintw ( m_InWin,
                NConfig::SCORES_SHOW_FIRST_LINE + headerIndent + contentIndent ++,
                NConfig::SCORES_SHOW_LEFT_INDT,
                NConfig::NO_SCORES );

    mvwprintw ( m_InWin,
                NConfig::SCORES_SHOW_FIRST_LINE + headerIndent + contentIndent,
                NConfig::SCORES_SHOW_LEFT_INDT,
                NConfig::SCORES_DELIMITER );
  }
  else
  {
    range = m_InWinH - headerIndent - 6;

    while ( true )
    {
      /* Scrolling. */
      if ( input == KEY_UP && shift > 0 )
        -- shift;
      if ( input == KEY_DOWN && shift < ( s_Scores . size ( ) - range ) && s_Scores . size ( ) > range )
        ++ shift;
      if ( input != KEY_UP && input != KEY_DOWN )
        break;

      contentIndent = 0;
      for ( size_t idx = 0 + shift; idx < s_Scores . size ( ) && ( idx - shift) < range; ++ idx )
      {
        ClearInWinLine ( { (int) ( NConfig::SCORES_SHOW_FIRST_LINE + headerIndent + contentIndent ) } );
        mvwprintw ( m_InWin,
                    NConfig::SCORES_SHOW_FIRST_LINE + headerIndent + contentIndent ++,
                    NConfig::SCORES_SHOW_LEFT_INDT,
                    NConfig::SCORES_LINE, s_Scores[idx] . m_BoardName . c_str ( ),
                    s_Scores[idx] . m_Score,
                    s_Scores[idx] . m_Won . c_str ( ) );
      }
      ClearInWinLine ( { (int) ( NConfig::SCORES_SHOW_FIRST_LINE + headerIndent + contentIndent ) } );
      mvwprintw ( m_InWin,
                  NConfig::SCORES_SHOW_FIRST_LINE + headerIndent + contentIndent,
                  NConfig::SCORES_SHOW_LEFT_INDT,
                  NConfig::SCORES_DELIMITER );
      input = wgetch ( m_InWin );
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
