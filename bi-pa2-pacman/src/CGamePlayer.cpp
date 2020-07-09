#include "CGamePlayer.hpp"

//----------------------------------------------------------------------------------------------------------------------
CGamePlayer::CGamePlayer ( WINDOW * inWIn, int inWinH, int inWinW ) :
    CContext      ( inWIn, inWinH, inWinW ),
    m_PMRem       ( NConfig::LIVES ),
    m_ToEat       ( 0 ),
    m_Iter        ( 0 ),
    m_GameMode    ( EGameMode::CHASE ),
    m_Score       ( 0 ),
    m_GhEaten     ( 0 )
{
  m_BrdName[NConfig::INPUT_MAX_LEN] = '\0';
}
//----------------------------------------------------------------------------------------------------------------------
void CGamePlayer::Run ( void )
{
  InitInWin ( NConfig::HEAD_PLAY_GAME );

  if ( ! PrepareGame ( ) )
    return;

  mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_S_LINE, NConfig::Indent ( m_InWinW, strlen ( NConfig::ENTER_START ) ), NConfig::ENTER_START );
  wrefresh ( m_InWin );
  WaitForEnter ( );
  ClearInWinLine ( { NConfig::GP_DIALOGUE_S_LINE } );
  mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_S_LINE, NConfig::Indent ( m_InWinW, strlen ( NConfig::ENTER_END ) ), NConfig::ENTER_END );
  wrefresh ( m_InWin );

  nodelay ( m_InWin, true );
  PlayGame ( );
  nodelay ( m_InWin, false );

  ClearInWinLine ( { NConfig::GP_DIALOGUE_S_LINE } );
  mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_S_LINE, NConfig::Indent ( m_InWinW, strlen ( NConfig::ANY_KEY_EXIT ) ), NConfig::ANY_KEY_EXIT );
  wrefresh ( m_InWin );
  ExitInWin ( );
}
//----------------------------------------------------------------------------------------------------------------------
bool CGamePlayer::PrepareGame ( void )
{
  if ( ! s_Boards . size ( ) )
  {
    mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_Q_LINE, 1, NConfig::NO_MAZE_AVAILABLE );
    ExitInWin ( );
    return false;
  }
  
  while ( true )
  {
    mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_Q_LINE, 1, NConfig::PLEASE_ENTER_MAZE_NAME, NConfig::INPUT_MAX_LEN );
    SuggestMaze ( );
    Visible ( );
    mvwgetnstr ( m_InWin, NConfig::GP_DIALOGUE_I_LINE, 1, m_BrdName, NConfig::INPUT_MAX_LEN );
    Invisible ( );
    wrefresh ( m_InWin );

    
    if ( CContext::s_Boards . find ( m_BrdName ) == CContext::s_Boards . end ( ) )
    {
      mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_R_LINE, 1, NConfig::NO_SUCH_MAZE );
      ClearInWinLine ( { NConfig::GP_DIALOGUE_Q_LINE, NConfig::GP_DIALOGUE_I_LINE } );
      wrefresh ( m_InWin );
    }
    else
      break;
  }

  ClearInWinLine ( { NConfig::GP_DIALOGUE_I_LINE } );
  mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_R_LINE, 1, NConfig::INPUT_MAZE_SUCCESS );
  m_BrdIndW = NConfig::Indent ( m_InWinW, CContext::s_Boards[m_BrdName] . GetMaxX ( ) );
  m_BrdIndH = NConfig::Indent ( m_InWinH, CContext::s_Boards[m_BrdName] . GetMaxY ( ) );
  wgetch ( m_InWin );


  s_Boards[m_BrdName] . CleanForNewGame ( false );
  m_Score = m_GhEaten = m_Iter = 0;
  m_PMRem = NConfig::LIVES;
  m_ToEat = s_Boards[m_BrdName] . GetNmbToEat ( );
  m_GameMode = EGameMode::CHASE;

  RenderDynamicInfo ( );

  CContext::s_Boards[m_BrdName] . Render ( m_InWin, m_BrdIndW, m_BrdIndH );
  ClearInWinLine ( { NConfig::GP_DIALOGUE_Q_LINE, NConfig::GP_DIALOGUE_S_LINE, NConfig::GP_DIALOGUE_R_LINE } );
  ResetCharacters ( );
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CGamePlayer::SuggestMaze ( void )
{
  size_t indent = 1;
  mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_S_LINE, indent, NConfig::FOR_EXAMPLE . c_str ( ) );
  indent += NConfig::FOR_EXAMPLE . size ( );
  mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_S_LINE, indent, s_Boards . cbegin ( ) -> first . c_str ( ) );
  indent += s_Boards . cbegin ( ) -> first . size ( );
  mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_S_LINE, indent, ", ..." );
  wrefresh ( m_InWin );
}
//----------------------------------------------------------------------------------------------------------------------
void CGamePlayer::ResetCharacters ( void )
{
  int dummy = 0;
  m_Characters . clear ( );
  m_Characters . emplace_back ( new CPacMan ( s_Boards[m_BrdName],
                                              dummy,
                                              s_Boards[m_BrdName] . GetPacManStart ( ),
                                              dummy,
                                              s_Boards[m_BrdName] . GetGhStart ( )[0] ) );
  m_Characters . emplace_back ( new CBlinky ( s_Boards[m_BrdName],
                                              m_Characters[0] -> GetDir ( ),
                                              m_Characters[0] -> GetPos ( ),
                                              dummy,
                                              s_Boards[m_BrdName] . GetGhStart ( )[0] ) );
  m_Characters . emplace_back ( new CPinky  ( s_Boards[m_BrdName],
                                              m_Characters[0] -> GetDir ( ),
                                              m_Characters[0] -> GetPos ( ),
                                              m_Characters[1] -> GetDir ( ),
                                              m_Characters[1] -> GetPos ( ) ) );
  m_Characters . emplace_back ( new CInky   ( s_Boards[m_BrdName],
                                              m_Characters[0] -> GetDir ( ),
                                              m_Characters[0] -> GetPos ( ),
                                              m_Characters[1] -> GetDir ( ),
                                              m_Characters[1] -> GetPos ( ) ) );
  m_Characters . emplace_back ( new CClyde  ( s_Boards[m_BrdName],
                                              m_Characters[0] -> GetDir ( ),
                                              m_Characters[0] -> GetPos ( ),
                                              m_Characters[1] -> GetDir ( ),
                                              m_Characters[1] -> GetPos ( ) ) );
}//----------------------------------------------------------------------------------------------------------------------
void CGamePlayer::PlayGame ( void )
{
  while ( m_ToEat > 0 )
  {
    int input = wgetch ( m_InWin );
    switch ( input )
    {
      case KEY_UP:
        m_Characters[0] -> SetDir ( NConfig::DIR_UP );
        wrefresh ( m_InWin );
        break;
      case KEY_RIGHT:
        m_Characters[0] -> SetDir ( NConfig::DIR_RIGHT );
        break;
      case KEY_DOWN:
        m_Characters[0] -> SetDir ( NConfig::DIR_DOWN );
        break;
      case KEY_LEFT:
        m_Characters[0] -> SetDir ( NConfig::DIR_LEFT );
        wrefresh ( m_InWin );
        break;
      case NConfig::KEY_ENTER_BIG:
        ShowAndStoreGameResult ( );
        return;
      default:
        break;
    }
    UpdateGameMode ( );
    RenderDynamicInfo ( );
    if ( ! MoveAll ( ) )
      break;
    std::this_thread::sleep_for ( std::chrono::milliseconds ( NConfig::SLEEP_TIME ) );
  }
  ShowAndStoreGameResult ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CGamePlayer::UpdateGameMode ( void )
{
  ++ m_Iter;
  if ( m_GameMode == EGameMode::SCATTER && m_Iter == CContext::s_Boards[m_BrdName] . GetScttrTmr ( ) * NConfig::SLEEP_TIME )
  {
    CContext::s_Boards[m_BrdName] . SetCherry ( m_InWin, m_BrdIndW, m_BrdIndH );
    m_GameMode = EGameMode::CHASE;
    TurnGhosts ( );
    m_Iter = 0;
  }
  else if ( m_GameMode == EGameMode::CHASE
           && m_Iter == CContext::s_Boards[m_BrdName] . GetChaseTmr ( ) * NConfig::SLEEP_TIME )
  {
    m_GameMode = EGameMode::SCATTER;
    TurnGhosts ( );
    m_Iter = 0;
  }
  else if ( m_GameMode == EGameMode::FRIGHTENED
              && m_Iter == CContext::s_Boards[m_BrdName] . GetFrghtTmr ( ) * NConfig::SLEEP_TIME )
  {
    m_GameMode = EGameMode::SCATTER;
    TurnGhosts ( );
    CharactersFromFrght ( );
    m_Iter = 0;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CGamePlayer::CharactersToFrght ( void )
{
  for ( auto & character : m_Characters )
  {
    character -> ToFrght ( );
    character -> RenderChangedTiles ( m_InWin, m_InWinW, m_InWinH );
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CGamePlayer::CharactersFromFrght ( void )
{
  for ( auto & character : m_Characters )
  {
    character -> FromFrght ( );
    character -> RenderChangedTiles ( m_InWin, m_InWinW, m_InWinH );
  }
}
//----------------------------------------------------------------------------------------------------------------------
bool CGamePlayer::MoveAll ( void )
{
  bool ret = true;
  switch ( m_GameMode )
  {
    case EGameMode::SCATTER:
      for ( auto & character : m_Characters )
        if ( ! ( ret = UpdateDynamicInfo ( character -> MoveScttr ( ) ) ) )
          break;
      break;
    case EGameMode::CHASE:
      for ( auto & character : m_Characters )
        if ( ! ( ret = UpdateDynamicInfo ( character -> MoveChase ( ) ) ) )
          break;
      break;
    case EGameMode::FRIGHTENED:
      for ( auto & character : m_Characters )
        if ( ! ( ret = UpdateDynamicInfo ( character -> MoveFrght ( ) ) ) )
          break;
      break;
  }

  for ( const auto & character : m_Characters )
    character -> RenderChangedTiles ( m_InWin, m_InWinW, m_InWinH );
  return ret;
}
//----------------------------------------------------------------------------------------------------------------------
void CGamePlayer::TurnGhosts ( void )
{
  for ( size_t i = 1; i < m_Characters . size ( ); ++ i )
    m_Characters[i] -> TurnBack ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CGamePlayer::RenderDynamicInfo ( void )
{
  ClearInWinLine ( { NConfig::Indent ( m_InWinH, s_Boards[m_BrdName] . GetMaxY ( ) ) + 2,
                     NConfig::Indent ( m_InWinH, s_Boards[m_BrdName] . GetMaxY ( ) ) + 3 } );
  mvwprintw ( m_InWin, NConfig::Indent ( m_InWinH, s_Boards[m_BrdName] . GetMaxY ( ) ) + 2,
              NConfig::Indent ( m_InWinW,
                                strlen ( "Score: xxxx" ) ) + NConfig::INWINSHIFTW,
                                "Score: %04d", m_Score );

  wattron ( m_InWin, A_BOLD | COLOR_PAIR ( 3 ) );
  for ( int i = 1; i < m_PMRem; ++ i )
    mvwprintw ( m_InWin,
                NConfig::Indent ( m_InWinH, s_Boards[m_BrdName] . GetMaxY ( ) ) + 3,
                NConfig::Indent ( m_InWinW, 1 ) + i,
                "C" );
  wattroff ( m_InWin, A_BOLD | COLOR_PAIR ( 3 ) );
  wrefresh ( m_InWin );
}
//----------------------------------------------------------------------------------------------------------------------
bool CGamePlayer::UpdateDynamicInfo ( int retVal )
{
  int dummy = 0;
  switch ( retVal )
  {
    case NConfig::DOT_EATEN:
      m_Score += NConfig::DOT_EATEN;
      m_ToEat -= 1;
      break;

    case NConfig::BONUS_EATEN:
      m_Score += NConfig::BONUS_EATEN;
      m_ToEat -= 1;
      m_GameMode = EGameMode::FRIGHTENED;
      CharactersToFrght ( );
      TurnGhosts ( );
      m_GhEaten = 0;
      m_Iter = 0;
      break;

    case NConfig::CHERRY_EATEN:
      m_Score += NConfig::CHERRY_EATEN;
      break;

    case NConfig::BLINKY_EATEN:
      m_Score += NConfig::GHOST_EATEN * ++ m_GhEaten;
      m_Characters[1] = std::unique_ptr<CCharacter> ( new CBlinky ( s_Boards[m_BrdName],
                                                                    m_Characters[0] -> GetDir ( ),
                                                                    m_Characters[0] -> GetPos ( ),
                                                                    dummy,
                                                                    s_Boards[m_BrdName] . GetGhStart ( )[0] ) );
      s_Boards[m_BrdName] . Render ( m_InWin, m_BrdIndW, m_BrdIndH );
      break;

    case NConfig::PINKY_EATEN:
      m_Score += NConfig::GHOST_EATEN * ++ m_GhEaten;
      m_Characters[2] = std::unique_ptr<CCharacter> ( new CPinky ( s_Boards[m_BrdName],
                                                                   m_Characters[0] -> GetDir ( ),
                                                                   m_Characters[0] -> GetPos ( ),
                                                                   m_Characters[1] -> GetDir ( ),
                                                                   m_Characters[1] -> GetPos ( ) ) );
      s_Boards[m_BrdName] . Render ( m_InWin, m_BrdIndW, m_BrdIndH );
      break;

    case NConfig::INKY_EATEN:
      m_Score += NConfig::GHOST_EATEN * ++ m_GhEaten;
      m_Characters[3] = std::unique_ptr<CCharacter> ( new CInky ( s_Boards[m_BrdName],
                                                                  m_Characters[0] -> GetDir ( ),
                                                                  m_Characters[0] -> GetPos ( ),
                                                                  m_Characters[1] -> GetDir ( ),
                                                                  m_Characters[1] -> GetPos ( ) ) );
      s_Boards[m_BrdName] . Render ( m_InWin, m_BrdIndW, m_BrdIndH );
      break;

    case NConfig::CLYDE_EATEN:
      m_Score += NConfig::GHOST_EATEN * ++ m_GhEaten;
      m_Characters[4] = std::unique_ptr<CCharacter> ( new CClyde  ( s_Boards[m_BrdName],
                                                                    m_Characters[0] -> GetDir ( ),
                                                                    m_Characters[0] -> GetPos ( ),
                                                                    m_Characters[1] -> GetDir ( ),
                                                                    m_Characters[1] -> GetPos ( ) ) );
      s_Boards[m_BrdName] . Render ( m_InWin, m_BrdIndW, m_BrdIndH );
      break;

    case NConfig::PACMAN_EATEN:
      m_PMRem -= 1;
      RenderDynamicInfo ( );
      if ( ! m_PMRem )
        return false;
      s_Boards[m_BrdName] . CleanForNewGame ( true );
      ResetCharacters ( );
      s_Boards[m_BrdName] . Render ( m_InWin, m_BrdIndW, m_BrdIndH );

      ClearInWinLine ( { NConfig::GP_DIALOGUE_S_LINE } );
      mvwprintw ( m_InWin, NConfig::GP_DIALOGUE_S_LINE, NConfig::Indent ( m_InWinW, strlen ( NConfig::ENTER_RESUME ) ), NConfig::ENTER_RESUME );
      wrefresh ( m_InWin );
      WaitForEnter ( );
      ClearInWinLine ( { NConfig::GP_DIALOGUE_S_LINE } );
      break;

    default:
      break;
  }
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CGamePlayer::ShowAndStoreGameResult ( void )
{
  ClearInWinLine ( { 5 } );
  if ( ! m_ToEat )
  {
    wattron ( m_InWin, A_BOLD | COLOR_PAIR ( 2 ) );
    mvwprintw ( m_InWin, 5, NConfig::Indent ( m_InWinW, strlen ( NConfig::WON ) ), NConfig::WON );
    wrefresh ( m_InWin );
    wattroff ( m_InWin, A_BOLD | COLOR_PAIR ( 2 ) );
    s_Scores . emplace_back ( m_BrdName, m_Score, "YES" );
  }
  else
  {
    wattron ( m_InWin, A_BOLD | COLOR_PAIR ( 1 ) );
    mvwprintw ( m_InWin, 5, NConfig::Indent ( m_InWinW, strlen ( NConfig::LOST ) ), NConfig::LOST );
    wrefresh ( m_InWin );
    wattroff ( m_InWin, A_BOLD | COLOR_PAIR ( 1 ) );
    s_Scores . emplace_back ( m_BrdName, m_Score, "NO" );
  }
}
//----------------------------------------------------------------------------------------------------------------------
