#include "CContext.hpp"

std::map<std::string, CGameBoard> CContext::s_Boards = std::map<std::string, CGameBoard> ( );
std::vector<TScore> CContext::s_Scores = std::vector<TScore> ( );
//----------------------------------------------------------------------------------------------------------------------
CContext::CContext ( WINDOW * inWin, int inWinH, int inWinW ) :
    m_InWin  ( inWin ),
    m_InWinH ( inWinH ),
    m_InWinW ( inWinW ) { }
//----------------------------------------------------------------------------------------------------------------------
void CContext::ClearInWinLine ( const std::vector<int> & lines )
{
  for ( const auto line : lines )
  {
    wmove ( m_InWin, line, 0 );
    wclrtoeol ( m_InWin );
  }
  box ( m_InWin, 0, 0 );
  wrefresh ( m_InWin );
}
//----------------------------------------------------------------------------------------------------------------------
void CContext::Visible ( void )
{
  echo ( );
  curs_set ( 1 );
}
//----------------------------------------------------------------------------------------------------------------------
void CContext::Invisible ( void )
{
  noecho ( );
  curs_set ( 0 );
}
//----------------------------------------------------------------------------------------------------------------------
void CContext::WaitForEnter ( void )
{
  while ( ( wgetch ( m_InWin ) ) != NConfig::KEY_ENTER_BIG ) { }
}
//----------------------------------------------------------------------------------------------------------------------
void CContext::InitInWin ( const char * title )
{
  werase ( m_InWin );
  box ( m_InWin, 0, 0 );
  mvwprintw ( m_InWin, 1, NConfig::Indent ( m_InWinW, strlen ( title ) ) + NConfig::INWINSHIFTW, "%s", title );
  wrefresh ( m_InWin );
}
//----------------------------------------------------------------------------------------------------------------------
void CContext::ExitInWin ( void )
{
  WaitForEnter ( );
  wclear       ( m_InWin );
  wrefresh     ( m_InWin );
}
//----------------------------------------------------------------------------------------------------------------------
