#include <iostream>
#include <cstring>
#include <cmath>
#include <thread>
#include <vector>
#include <set>
#include <queue>
#include <functional>
#include <climits>

#define SLEEP_FOR(time) this_thread::sleep_for ( time );

using namespace std;

class CGraph
{
public:
       CGraph       ( int nmbAgts );
  void GetInput     ( void );
  void Initialise   ( ostream & os );
  void AStar        ( ostream & os );
  void TotalResults ( ostream & os );

private:
  enum class ENodeVal
  {
    WALL     = 'X',
    START    = 'S',
    END      = 'E',
    FRESH    = ' ',
    OPEN     = '#',
    RESERVED = 'r',
    PATH     = 'o',
    NEWLINE  = '\n'
  };
  struct TNode
  {
  public:
    int      m_X;    // x-coordinate
    int      m_Y;    // y-coordinate
    ENodeVal m_N;    // node value
    int      m_PX;   // predecessor's x-coordinate
    int      m_PY;   // predecessor's y-coordinate
    int      m_SX;   // successor's x-coordinate
    int      m_SY;   // successor's y-coordinate
    double   m_EDS;  // euclidean distance from the start
    double   m_EDE;  // euclidean distance from the end
    double   m_Tot;  // m_EDS + m_EDE (CA*)
    size_t   m_TM;   // timestamp (in which time the node was reached)
    bool     m_Wait; // indicator whether the agent has already waited here or not

    TNode     ( int x, int y, ENodeVal n );
  };
  struct TCmp
  {
    bool operator ( ) ( const TNode * a, const TNode * b );
  };
  vector<pair<int, int>>                         m_S;     // start
  vector<pair<int, int>>                         m_E;     // end
  vector<vector<vector<TNode>>>                  m_V;     // vector of mazes (one for each agent)
  vector<int>                                    m_Exp;   // number of expanded nodes
  vector<int>                                    m_Len;   // path length
  vector<int>                                    m_Wait;  // number of waitings during path
  priority_queue<TNode *, vector<TNode *>, TCmp> m_PQ;    // priority queue of pointers to nodes
  int                                            m_Max_X;
  int                                            m_Max_y;
  vector<bool>                                   m_Succ;
  int                                            m_NmbAgts;
  size_t                                         m_Proc;  // index of agent currently being processed
  void             AStarExpand        ( int x, int y, int px, int py, ostream & os );
  void             Heuristics         ( void );
  void             Explanation        ( ostream & os ) const;
  void             Print              ( ostream & os ) const;
  void             DrawPathForwards   ( ostream & os );
  void             DrawPathBackwards  ( ostream & os );
  friend ostream & operator <<        ( ostream & os, const ENodeVal & x );
  friend ostream & operator <<        ( ostream & os, const CGraph & x );
};
//======================================================================================================================
CGraph::TNode::TNode ( int x, int y, ENodeVal n ) :
    m_X    ( x ),
    m_Y    ( y ),
    m_N    ( n ),
    m_PX   ( INT_MAX ),
    m_PY   ( INT_MAX ),
    m_SX   ( INT_MAX ),
    m_SY   ( INT_MAX ),
    m_EDS  ( 0 ),
    m_EDE  ( 0 ),
    m_Tot  ( m_EDS + m_EDE ),
    m_TM   ( 0 ),
    m_Wait ( false ){ }
//======================================================================================================================
bool CGraph::TCmp::operator ( ) ( const TNode * a, const TNode * b ) { return a->m_Tot > b->m_Tot; }
//======================================================================================================================
CGraph::CGraph ( int nmbAgts ) :
    m_Exp     ( 0 ),
    m_Len     ( 0 ),
    m_Max_X   ( 0 ),
    m_Max_y   ( 0 ),
    m_NmbAgts ( nmbAgts ),
    m_Proc    ( 0 ) { }
//----------------------------------------------------------------------------------------------------------------------
void CGraph::GetInput ( void )
{
  char c;
  int i = 0, x = 0, y = 0;
  string dummy;
  pair<int, int> tmp_S, tmp_E;
  m_V . emplace_back ( vector <vector<TNode>> ( ) ); // maze for the first agent
  m_V[0].emplace_back ( vector <TNode> ( ) );
  cin >> noskipws;
  while ( cin >> c && c != 's' )
  {
    m_V[0][i].emplace_back ( TNode ( x++, y, ( ENodeVal ) c ) );
    if ( c == '\n' )
    {
      m_Max_X = x;
      m_V[0].emplace_back ( vector<TNode> ( ) );
      x = 0;
      i++;
      y++;
    }
  }

  vector<vector<TNode>> clean_maze = m_V[0];
  m_Max_y = y;
  cin >> skipws;
  for ( int j = 0; j < m_NmbAgts; ++ j )
  {
    cin >> dummy >> tmp_S.first >> c >> tmp_S.second;
    cin >> dummy >> tmp_E.first >> c >> tmp_E.second;
    cout << tmp_S.first << tmp_S.second << endl;
    m_S    . emplace_back ( tmp_S );
    m_E    . emplace_back ( tmp_E );
    m_Succ . emplace_back ( false );
    m_Exp  . emplace_back ( 0 );
    m_Len  . emplace_back ( 0 );
    m_Wait . emplace_back ( 0 );
    if ( j != 0 )
      m_V . emplace_back ( clean_maze ); // maze for other agents
    m_V[j][m_S[j].second][m_S[j].first].m_N  = ENodeVal::START; // !!! attention - order of coordinates
    m_V[j][m_E[j].second][m_E[j].first].m_N  = ENodeVal ::END;  // !!! attention - order of coordinates
    m_V[j][m_S[j].second][m_S[j].first].m_TM = 0;
    m_V[j][m_S[j].second][m_S[j].first].m_PX = m_S[j].first;
    m_V[j][m_S[j].second][m_S[j].first].m_PY = m_S[j].second;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::Initialise ( ostream & os )
{
  for ( ; m_Proc < m_S . size ( ); ++ m_Proc )
  {
    m_Succ[m_Proc] = false;
    AStar ( os );
  }
  for ( m_Proc = 0; m_Proc < m_S . size ( ); ++ m_Proc )
  {
    if ( m_Len[m_Proc] )
    {
      m_Len[m_Proc]  = 0;
      DrawPathForwards ( os );
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::AStar ( ostream & os )
{
  Heuristics ( );
  int tmp_x, tmp_y;
  m_Exp[m_Proc] ++;
  m_PQ.push ( & m_V[m_Proc][m_S[m_Proc].second][m_S[m_Proc].first] );
  AStarExpand ( m_S[m_Proc].first, m_S[m_Proc].second, m_S[m_Proc].first, m_S[m_Proc].second, os );

  while ( ! m_PQ.empty ( ) )
  {
    tmp_x = m_PQ.top ( )->m_X;
    tmp_y = m_PQ.top ( )->m_Y;
    m_PQ.pop ( );
    AStarExpand ( tmp_x + 1, tmp_y, tmp_x, tmp_y, os );
    AStarExpand ( tmp_x, tmp_y + 1, tmp_x, tmp_y, os );
    AStarExpand ( tmp_x - 1, tmp_y, tmp_x, tmp_y, os );
    AStarExpand ( tmp_x, tmp_y - 1, tmp_x, tmp_y, os );
  }
  if ( m_Succ[m_Proc] )
    DrawPathBackwards ( os );
  else
    os << "\033[31mPATH NOT FOUND.\033[0m" << endl;
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::TotalResults ( ostream & os )
{
  os << "---------------------------------------" << endl;
  os << "---------------------------------------" << endl;
  os << "            Total results              " << endl;
  for ( int i = 0; i < m_NmbAgts; ++ i )
  {
    os << "Agent " << i + 1 << ":" << endl;
    os << "  - nodes expanded: " << m_Exp[i] << endl;
    os << "  - waitings performed: " << m_Wait[i] << endl;
    if ( m_Len[i] > 0 )
      os << "  - path length: " << m_Len[i] << endl;
    else if ( m_S[m_Proc].first == m_E[m_Proc].first && m_S[m_Proc].second == m_E[m_Proc].second )
      os << "  - path length: 0 (start and end are the same nodes)" << endl;
    else
      os << "    Path does not exist."  << endl;
    os << "---------------------------------------" << endl;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::AStarExpand ( int x, int y, int px, int py, ostream & os )
{
  if ( m_V[m_Proc][y][x].m_N == ENodeVal::END )
  {
    if ( ! ( m_S[m_Proc].first == m_E[m_Proc].first && m_S[m_Proc].second == m_E[m_Proc].second ) )
      m_Exp[m_Proc] ++;
    m_Succ[m_Proc] = true;
    m_V[m_Proc][y][x].m_PX = px;
    m_V[m_Proc][y][x].m_PY = py;
    m_V[m_Proc][y][x].m_TM  = m_V[m_Proc][py][px].m_TM + 1;
    while ( ! m_PQ.empty ( ) )
      m_PQ.pop ( );
    return;
  }

  if ( m_V[m_Proc][y][x].m_N == ENodeVal::FRESH && ! m_Succ[m_Proc] )
  {
    m_V[m_Proc][y][x].m_TM  = m_V[m_Proc][py][px].m_TM + 1;

    for ( int i = 0; i < m_NmbAgts; ++ i )
    {
      if ( m_V[i][y][x].m_N == ENodeVal::RESERVED && m_V[i][y][x].m_TM == m_V[m_Proc][y][x].m_TM )
      {
        if ( ! m_V[m_Proc][y][x].m_Wait )
        {
          m_V[m_Proc][y][x].m_TM += 1;
          m_V[m_Proc][y][x].m_Wait = true;
          m_Wait[m_Proc] += 1;
        }
        else
        {
          m_V[m_Proc][y][x].m_TM = 0;
          return;
        }
      }
    }
    m_Exp[m_Proc] ++;
    m_V[m_Proc][y][x].m_N  = ENodeVal::OPEN;
    m_V[m_Proc][y][x].m_PX = px;
    m_V[m_Proc][y][x].m_PY = py;
    Print ( os );
    m_PQ.push ( & m_V[m_Proc][y][x] );
  }

  if ( m_V[m_Proc][y][x].m_N == ENodeVal::START )
  {
    if ( ! m_V[m_Proc][y][x].m_Wait
      && ( m_V[m_Proc][y][x+1].m_N == ENodeVal::WALL || m_V[m_Proc][y][x+1].m_N == ENodeVal::END || ( m_V[m_Proc][y][x+1].m_N == ENodeVal::RESERVED && m_V[m_Proc][y][x+1].m_TM == m_V[m_Proc][y][x].m_TM + 1 ) )
      && ( m_V[m_Proc][y][x-1].m_N == ENodeVal::WALL || m_V[m_Proc][y][x-1].m_N == ENodeVal::END || ( m_V[m_Proc][y][x-1].m_N == ENodeVal::RESERVED && m_V[m_Proc][y][x-1].m_TM == m_V[m_Proc][y][x].m_TM + 1 ) )
      && ( m_V[m_Proc][y+1][x].m_N == ENodeVal::WALL || m_V[m_Proc][y+1][x].m_N == ENodeVal::END || ( m_V[m_Proc][y+1][x].m_N == ENodeVal::RESERVED && m_V[m_Proc][y+1][x].m_TM == m_V[m_Proc][y][x].m_TM + 1 ) )
      && ( m_V[m_Proc][y-1][x].m_N == ENodeVal::WALL || m_V[m_Proc][y-1][x].m_N == ENodeVal::END || ( m_V[m_Proc][y-1][x].m_N == ENodeVal::RESERVED && m_V[m_Proc][y-1][x].m_TM == m_V[m_Proc][y][x].m_TM + 1 ) ) )
    {
      m_V[m_Proc][y][x].m_Wait = true;
      m_V[m_Proc][x][x].m_TM += 1;
      m_Wait[m_Proc] += 1;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::Heuristics ( void )
{
  for ( auto & i : m_V[m_Proc] )
    for ( auto & j : i )
      if ( j.m_N != ENodeVal::WALL )
      {
        j.m_EDS = sqrt ( pow ( j.m_X - m_S[m_Proc].first, 2 ) + pow ( j.m_Y - m_S[m_Proc].second, 2 ) );
        j.m_EDE = sqrt ( pow ( j.m_X - m_E[m_Proc].first, 2 ) + pow ( j.m_Y - m_E[m_Proc].second, 2 ) );
        j.m_Tot = 0.9 * j.m_EDS + j.m_EDE;
      }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::Explanation ( ostream & os ) const
{
  os << "---------------------------------------" << endl;
  os << "\033[34mS Start\033[0m" << endl;
  os << "\033[35mE End\033[0m" << endl;
  os << "\033[33m# Opened node\033[0m" << endl;
  os << "\033[36mr Reserved node\033[0m" << endl;
  os << "\033[32mo Path\033[0m" << endl;
  os << "\033[31mX Wall\033[0m" << endl;
  os << "space Fresh node" << endl;
  os << "---------------------------------------" << endl;
  os << "Currently running Agent number " << m_Proc + 1 << endl;
  os << "  - nodes expanded: " << m_Exp[m_Proc] << endl;
  os << "  - waitings performed: " << m_Wait[m_Proc] << endl;
  os << "  - path length: " << m_Len[m_Proc] << endl;
}
//----------------------------------------------------------------------------------------------------------------------
inline void CGraph::Print ( ostream & os ) const
{
  os << "\033[2J\033[1;1H";
  os << *this;
  SLEEP_FOR ( 0.01s )
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::DrawPathForwards ( ostream & os )
{
  Print ( os );
  int pos_x = m_S[m_Proc].first, pos_y = m_S[m_Proc].second;
  int succ_x = m_V[m_Proc][pos_y][pos_x].m_SX, succ_y = m_V[m_Proc][pos_y][pos_x].m_SY;
  while ( ! ( succ_x == m_E[m_Proc].first && succ_y == m_E[m_Proc].second ) )
  {
    succ_x = m_V[m_Proc][pos_y][pos_x].m_SX;
    succ_y = m_V[m_Proc][pos_y][pos_x].m_SY;
    if ( succ_x == INT_MAX || succ_y == INT_MAX )
      return;
    if ( m_V[m_Proc][succ_y][succ_x].m_N != ENodeVal::END )
      m_V[m_Proc][succ_y][succ_x].m_N = ENodeVal::PATH;
    m_Len[m_Proc] ++;
    Print ( os );

    pos_x = succ_x;
    pos_y = succ_y;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::DrawPathBackwards ( ostream & os )
{
  Print ( os );
  int pos_x = m_E[m_Proc].first, pos_y = m_E[m_Proc].second;
  int pred_x = m_V[m_Proc][pos_y][pos_x].m_PX, pred_y = m_V[m_Proc][pos_y][pos_x].m_PY;
  while ( ! ( pred_x == m_S[m_Proc].first && pred_y == m_S[m_Proc].second ) )
  {
    pred_x = m_V[m_Proc][pos_y][pos_x].m_PX;
    pred_y = m_V[m_Proc][pos_y][pos_x].m_PY;

    if ( pred_x == INT_MAX || pred_y == INT_MAX )
      return;
    m_V[m_Proc][pred_y][pred_x].m_SX = pos_x;
    m_V[m_Proc][pred_y][pred_x].m_SY = pos_y;

    if ( m_V[m_Proc][pred_y][pred_x].m_N != ENodeVal::START )
      m_V[m_Proc][pred_y][pred_x].m_N = ENodeVal::RESERVED;
    m_Len[m_Proc] ++;
    Print ( os );

    pos_x = pred_x;
    pos_y = pred_y;
  }
}
//----------------------------------------------------------------------------------------------------------------------
ostream & operator << ( ostream & os, const CGraph::ENodeVal & x )
{
  os << ( char ) x;
  return os;
}
//----------------------------------------------------------------------------------------------------------------------
ostream & operator << ( ostream & os, const CGraph & g )
{
  bool processed = false;
  set<unsigned int> tmp;
  for ( int y = 0; y < g.m_Max_y; ++ y )
    for ( int x = 0; x < g.m_Max_X; ++ x )
    {
      for ( unsigned int i = 0; i <= g.m_Proc; ++ i )
        if ( g.m_V[i][y][x].m_N == CGraph::ENodeVal::PATH )
          tmp.insert ( i + 1 );
      if ( ! tmp.empty ( ) )
      {
        os << "\033[32m" << *(max_element ( tmp.begin ( ), tmp.end ( ) )) << "\033[0m";
        tmp.clear ( );
        continue;
      }

      processed = false;
      for ( int i = 0; i < g.m_NmbAgts; ++ i )
        if ( g.m_V[i][y][x].m_N == CGraph::ENodeVal::RESERVED )
        {
          os << "\033[36m" << CGraph::ENodeVal::RESERVED << "\033[0m";
          processed = true;
          break;
        }
      if ( processed )
        continue;

      for ( int i = 0; i < g.m_NmbAgts; ++ i )
        if ( g.m_V[i][y][x].m_N == CGraph::ENodeVal::START )
        {
          os << "\033[34m" << CGraph::ENodeVal::START << "\033[0m";
          processed = true;
          break;
        }
      if ( processed )
        continue;

      for ( int i = 0; i < g.m_NmbAgts; ++ i )
        if ( g.m_V[i][y][x].m_N == CGraph::ENodeVal::END )
        {
          os << "\033[35m" << CGraph::ENodeVal::END << "\033[0m";
          processed = true;
          break;
        }
      if ( processed )
        continue;

      switch ( g.m_V[g.m_Proc][y][x].m_N )
      {
        case CGraph::ENodeVal::START:
          os << "\033[34m" << CGraph::ENodeVal::START << "\033[0m";
          break;
        case CGraph::ENodeVal::END:
          os << "\033[35m" << CGraph::ENodeVal::END << "\033[0m";
          break;
        case CGraph::ENodeVal::WALL:
          os << "\033[31m" << CGraph::ENodeVal::WALL << "\033[0m";
          break;
        case CGraph::ENodeVal::OPEN:
          os << "\033[33m" << CGraph::ENodeVal::OPEN << "\033[0m";
          break;
        case CGraph::ENodeVal::RESERVED:
          os << "\033[36m" << CGraph::ENodeVal::RESERVED << "\033[0m";
          break;
        case CGraph::ENodeVal::PATH:
          os << "\033[32m" << g. m_Proc + 1 << "\033[0m";
          break;
        case CGraph::ENodeVal::NEWLINE:
          os << CGraph::ENodeVal::NEWLINE;
          break;
        default:
          os << CGraph::ENodeVal::FRESH;
      }
    }
  g.Explanation ( os );
  return os;
}

//======================================================================================================================
int main ( int argc, char * argv [] )
{
  if ( argc != 2 || stoi ( argv[1] ) < 1 || stoi ( argv[1] ) > 9 )
  {
    cout << "<usage> ./a.out <number of agents>(1-9) < <filename>" << endl;
    return 1;
  }
  CGraph graph ( stoi ( argv[1] ) );
  graph.GetInput ( );
  graph.Initialise ( cout );
  graph.TotalResults ( cout );
  return 0;
}
