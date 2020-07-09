#include "CGameManager.hpp"

//----------------------------------------------------------------------------------------------------------------------
CGameManager::CGameManager ( void ) :
    m_InWin  ( nullptr ),
    m_MMText { NConfig::HEAD_PLAY_GAME, NConfig::HEAD_LOAD_MAZE, NConfig::HEAD_SHOW_SCORE, NConfig::EXIT },
    m_MMH    ( m_MMText . size ( ) + 2 ),
    m_MMW    ( std::max_element ( m_MMText . begin ( ),
                                  m_MMText . end   ( ),
                                  [] ( const std::string & a, const std::string & b )
                                  { return a . size ( ) < b . size ( ); } ) -> size ( ) + 2 ) { }
//----------------------------------------------------------------------------------------------------------------------
void CGameManager::Run ( void )
{
  /* Initialise outer window */
  initscr  ( );
  start_color ( );
  DefineColors ( );
  raw      ( );                              /* Line buffering disabled (including signal keys). */
  noecho   ( );                              /* Don't echo received characters. */
  keypad   ( stdscr, true );                 /* Function and arrow keys enabled.*/
  curs_set ( 0 );                            /* Set cursor to invisible */
  getmaxyx ( stdscr, m_OutWinH, m_OutWinW ); /* Get max y and x coordinates of the window */
  signal ( SIGINT, SIG_IGN );                /* Disable Ctrl + C. */

  box ( stdscr, 0, 0 );
  attron ( A_BOLD | COLOR_PAIR ( 3 ) );
  mvprintw ( 1, NConfig::Indent ( m_OutWinW, strlen ( NConfig::HEAD_PACMAN ) ), NConfig::HEAD_PACMAN );
  attroff ( A_BOLD | COLOR_PAIR ( 3 ) );
  wrefresh ( stdscr );
  
  MainMenu ( );
  
  delwin ( stdscr );
  endwin ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CGameManager::MainMenu ( void )
{
  /* Initialise inner window and main menu options window. */
  m_InWin = newwin ( m_OutWinH * NConfig::INWINSIZE,
                     m_OutWinW * NConfig::INWINSIZE,
                     m_OutWinH * NConfig::INWININDENT,
                     m_OutWinW * NConfig::INWININDENT );
  keypad ( m_InWin, true );
  getmaxyx ( m_InWin, m_InWinH, m_InWinW );

  m_Subprgs . emplace_back ( std::unique_ptr<CContext> ( new CGamePlayer  ( m_InWin, m_InWinH, m_InWinW ) ) );
  m_Subprgs . emplace_back ( std::unique_ptr<CContext> ( new CMazeLoader  ( m_InWin, m_InWinH, m_InWinW ) ) );
  m_Subprgs . emplace_back ( std::unique_ptr<CContext> ( new CScoreShower ( m_InWin, m_InWinH, m_InWinW ) ) );

  WINDOW * optWin = newwin ( m_MMH, m_MMW, NConfig::Indent ( m_OutWinH, m_MMH ), NConfig::Indent ( m_OutWinW, m_MMW ) );
  keypad ( optWin, true );
  size_t highlight = 0;
  int navigate = 0;

  while ( ! ( highlight == m_MMText . size ( ) - 1 && navigate == NConfig::KEY_ENTER_BIG ) )
  {
    box ( m_InWin, 0, 0 );
    mvwprintw ( m_InWin, 1, NConfig::Indent ( m_InWinW + 2, strlen ( NConfig::HEAD_MAIN_MENU ) ), NConfig::HEAD_MAIN_MENU );
    wrefresh ( m_InWin );

    /* Line highlighting. */
    for ( size_t i = 0; i < m_MMText . size ( ); ++ i )
    {
      if ( highlight == i )
      {
        wattron ( optWin, A_BOLD | A_REVERSE );
        mvwprintw ( optWin, i + 1, NConfig::Indent ( m_MMW, m_MMText[i] . size ( ) ), m_MMText[i] . c_str ( ) );
        wattroff ( optWin, A_BOLD | A_REVERSE );
      }
      else
        mvwprintw ( optWin, i + 1, NConfig::Indent ( m_MMW, m_MMText[i] . size ( ) ), m_MMText[i] . c_str ( ) );
    }
    
    /* Navigating main menu. */
    navigate = wgetch ( optWin );
    switch ( navigate )
    {
      case KEY_UP:
        highlight = NConfig::Mod ( highlight - 1, m_MMText . size ( ) );
        break;
      case KEY_DOWN:
        highlight = NConfig::Mod ( highlight + 1, m_MMText . size ( ) );
        break;
      case NConfig::KEY_ENTER_BIG:
        werase   ( optWin );
        wrefresh ( optWin );
        werase   ( m_InWin );
        /* Exit which is the last element in main menu doesn't have a special subprogram. */
        if ( highlight < m_MMText . size ( ) - 1 )
          m_Subprgs[highlight] -> Run ( );
        break;
      default:
        break;
    }
  }
  
  delwin ( optWin );
  delwin ( m_InWin );
}
//----------------------------------------------------------------------------------------------------------------------
void CGameManager::DefineColors ( void )
{
  if ( has_colors ( ) == FALSE )
  {
    use_default_colors ( );
    
    init_pair ( 1, COLOR_WHITE, COLOR_BLACK );
    init_pair ( 2, COLOR_WHITE, COLOR_BLACK );
    init_pair ( 3, COLOR_WHITE, COLOR_BLACK );
    init_pair ( 4, COLOR_WHITE, COLOR_BLACK );
    init_pair ( 5, COLOR_WHITE, COLOR_BLACK );
    init_pair ( 6, COLOR_WHITE, COLOR_BLACK );
    init_pair ( 7, COLOR_WHITE, COLOR_BLACK );
  }
  else
  {
    init_color ( 60, 1000, 420, 720 ); /* COLOR_PINK */
    init_color ( 70, 1000, 560, 0   ); /* COLOR_ORANGE */
    
    init_pair  ( 1, COLOR_RED,     COLOR_BLACK );
    init_pair  ( 2, COLOR_GREEN,   COLOR_BLACK );
    init_pair  ( 3, COLOR_YELLOW,  COLOR_BLACK );
    init_pair  ( 4, COLOR_BLUE,    COLOR_BLACK );
    init_pair  ( 5, COLOR_CYAN,    COLOR_BLACK );
    init_pair  ( 6, 60,            COLOR_BLACK );
    init_pair  ( 7, 70,            COLOR_BLACK );
  }
}
//----------------------------------------------------------------------------------------------------------------------
