#include "CMazeLoader.hpp"

//----------------------------------------------------------------------------------------------------------------------
CMazeLoader::CMazeLoader ( WINDOW * inWin, int inWinH, int inWinW ) :
    CContext ( inWin, inWinH, inWinW ) { }
//----------------------------------------------------------------------------------------------------------------------
void CMazeLoader::Run ( void )
{
  InitInWin ( NConfig::HEAD_LOAD_MAZE );

  char inFileName[NConfig::INPUT_MAX_LEN + 1], boardName[NConfig::INPUT_MAX_LEN + 1];
  inFileName[NConfig::INPUT_MAX_LEN] = boardName[NConfig::INPUT_MAX_LEN] = '\0';

  while ( true )
  {
    mvwprintw ( m_InWin, NConfig::ML_DIALOGUE_Q_LINE, 1, NConfig::PLEASE_ENTER_MAZE_NAME, NConfig::INPUT_MAX_LEN );
    Visible ( );
    mvwgetnstr ( m_InWin, NConfig::ML_DIALOGUE_I_LINE, 1, boardName, NConfig::INPUT_MAX_LEN );
    Invisible ( );
    wrefresh ( m_InWin );
    if ( CContext::s_Boards . find ( boardName ) != CContext::s_Boards . end ( ) )
    {
      mvwprintw ( m_InWin, NConfig::ML_DIALOGUE_R_LINE, 1, NConfig::ALREADY_MAZE );
      ClearInWinLine ( { NConfig::ML_DIALOGUE_Q_LINE, NConfig::ML_DIALOGUE_I_LINE } );
    }
    else
      break;
  }
  
  ClearInWinLine ( { NConfig::ML_DIALOGUE_Q_LINE, NConfig::ML_DIALOGUE_I_LINE, NConfig::ML_DIALOGUE_R_LINE } );
  mvwprintw ( m_InWin, NConfig::ML_DIALOGUE_Q_LINE, 1, NConfig::PLEASE_ENTER_MAZE_PATH, NConfig::INPUT_MAX_LEN );
  Visible ( );
  mvwgetnstr ( m_InWin, NConfig::ML_DIALOGUE_I_LINE, 1, inFileName, NConfig::INPUT_MAX_LEN );
  Invisible ( );

  CGameBoard gameBoard;
  if ( gameBoard . LoadBoardData ( inFileName, m_InWin ) )
  {
    mvwprintw ( m_InWin, NConfig::ML_DIALOGUE_R_LINE, 1, NConfig::INPUT_MAZE_SUCCESS );
    wgetch ( m_InWin );
    ClearInWinLine ( { NConfig::ML_DIALOGUE_Q_LINE, NConfig::ML_DIALOGUE_I_LINE, NConfig::ML_DIALOGUE_R_LINE } );
    gameBoard . Render ( m_InWin, NConfig::Indent ( m_InWinW, gameBoard . GetMaxX ( ) ),
                         NConfig::Indent ( m_InWinH, gameBoard . GetMaxY ( ) ) );
    s_Boards[boardName] = gameBoard;
    s_Boards[boardName] . SetGameAttr ( );
  }
  else
    mvwprintw ( m_InWin, NConfig::ML_DIALOGUE_R_LINE, 1, NConfig::INPUT_MAZE_FAIL, inFileName );
  ExitInWin ( );
}
//----------------------------------------------------------------------------------------------------------------------
