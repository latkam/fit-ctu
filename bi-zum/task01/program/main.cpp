#include <iostream>
#include <cstring>
#include <climits>
#include <cmath>
#include <thread>
#include <vector>
#include <queue>
#include <functional>

#define SLEEP_FOR(time) this_thread::sleep_for ( time );

using namespace std;

class CGraph
{
public:
         CGraph       ( void );
    void GetInput     ( void );
    void RandomSearch ( void );
    void DFS          ( void );
    void BFS          ( void );
    void GreedySearch ( void );
    void Dijkstra     ( void );
    void AStar        ( void );

private:
    enum class ENodeVal
    {
        WALL    = 'X',
        START   = 'S',
        END     = 'E',
        FRESH   = ' ',
        OPEN    = '#',
        CLOSED  = 'c',
        PATH    = 'o',
        NEWLINE = '\n'
    };
    struct TNode
    {
    public:
        int      m_X;   // x-coordinate
        int      m_Y;   // y-coordinate
        ENodeVal m_N;   // node value
        int      m_PX;  // predecessor's x-coordinate
        int      m_PY;  // predecessor's y-coordinate
        double   m_ED;  // euclidean distance from the end (GreedySearch)
        double   m_ER;  // edge rating (Dijkstra)
        double   m_Tot; // m_ED + m_ER (A*)

        TNode     ( int x, int y, ENodeVal n );
    };
    struct TCmp
    {
        bool operator ( ) ( const TNode * a, const TNode * b );
    };
    pair <int, int>          m_S;     // start
    pair <int, int>          m_E;     // end
    vector <vector <TNode>>  m_V;     // vector of nodes
    int                      m_Exp;   // number of expanded nodes
    int                      m_Len;   // path length
    queue <TNode *>          m_Q;     // queue of pointers to nodes (use in BFS)
    priority_queue <TNode *, vector<TNode *>, TCmp> m_PQ; // priority queue of pointers to nodes (use in RandS and Dij)
    bool                     m_Stop;  // boolean indicating whether the searching is to be terminated
    vector <TNode *>         m_ToCnt; // vector of nodes to be expanded in the future (use in RandomSearch)
    int                      m_Max_X;
    int                      m_Max_y;

    void             RandomSearch_p     ( int x, int y );
    void             RandomSearchExpand ( int x, int y, int px, int py );
    void             DFS_p              ( int x, int y, int px, int py );
    void             BFSExpand          ( int x, int y, int px, int py );
    void             GreedySearchExpand ( int x, int y, int px, int py );
    void             DijkstraExpand     ( int x, int y, int px, int py );
    void             AStarExpand        ( int x, int y, int px, int py );
    void             Heuristics         ( void );
    void             Explanation        ( ostream & os ) const;
    void             Print              ( void ) const;
    void             DrawPath           ( void );
    friend ostream & operator <<        ( ostream & os, const ENodeVal & x );
    friend ostream & operator <<        ( ostream & os, const CGraph & x );
};
//======================================================================================================================
CGraph::TNode::TNode ( int x, int y, ENodeVal n ) :
        m_X   ( x ),
        m_Y   ( y ),
        m_N   ( n ),
        m_PX  ( INT_MAX ),
        m_PY  ( INT_MAX ),
        m_ED  ( 0 ),
        m_ER  ( m_N == ENodeVal::FRESH ? INT_MAX : 1 ),
        m_Tot ( m_ED + m_ER ) { }
//======================================================================================================================
bool CGraph::TCmp::operator ( ) ( const TNode * a, const TNode * b ) { return a->m_Tot > b->m_Tot; }
//======================================================================================================================
CGraph::CGraph ( void ) :
        m_Exp   ( 0 ),
        m_Len   ( 0 ),
        m_Stop  ( false ),
        m_Max_X ( 0 ),
        m_Max_y ( 0 ) { }
//----------------------------------------------------------------------------------------------------------------------
void CGraph::GetInput ( void )
{
    char c;
    int i = 0, x = 0, y = 0;
    string dummy;
    m_V.emplace_back ( vector <TNode> ( ) );
    cin >> noskipws;
    while ( cin >> c && c != 's' )
    {
        m_V[i].emplace_back ( TNode ( x++, y, ( ENodeVal ) c ) );
        if ( c == '\n' )
        {
            m_Max_X = x;
            m_V.emplace_back ( vector<TNode> ( ) );
            x = 0;
            i++;
            y++;
        }
    }
    m_Max_y = y;
    cin >> skipws;
    cin >> dummy >> m_S.first >> c >> m_S.second;
    cin >> dummy >> m_E.first >> c >> m_E.second;
    m_V[m_S.second][m_S.first].m_N = ENodeVal::START; // !!! attention - coordinates
    m_V[m_E.second][m_E.first].m_N = ENodeVal ::END; // !!! attention - coordinates
    m_V[m_S.second][m_S.first].m_ER = 0;
    m_V[m_S.second][m_S.first].m_PX = m_S.first;
    m_V[m_S.second][m_S.first].m_PY = m_S.second;
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::RandomSearch ( void )
{
    srand ( time ( nullptr ) );
    m_Exp++;
    RandomSearch_p ( m_S.first, m_S.second );
    DrawPath ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::RandomSearch_p ( int x, int y )
{
    if ( ( m_V[y][x].m_N != ENodeVal::FRESH
        && m_V[y][x].m_N != ENodeVal::START
        && m_V[y][x].m_N != ENodeVal::OPEN ) || m_Stop )
        return;

    RandomSearchExpand ( x + 1, y, x, y );
    RandomSearchExpand ( x, y + 1, x, y );
    RandomSearchExpand ( x - 1, y, x, y );
    RandomSearchExpand ( x, y - 1, x, y );

    if ( ! m_ToCnt.empty ( ) )
    {
        int rand_val = rand() % m_ToCnt.size ( );

        int tmp_x = m_ToCnt[rand_val]->m_X, tmp_y = m_ToCnt[rand_val]->m_Y;
        swap ( m_ToCnt[rand_val], m_ToCnt[m_ToCnt.size ( ) - 1] );
        m_ToCnt.pop_back ( );
        RandomSearch_p ( tmp_x, tmp_y );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::RandomSearchExpand ( int x, int y, int px, int py )
{
    if ( m_V[y][x].m_N == ENodeVal::END || m_V[y][x].m_N == ENodeVal::FRESH )
    {
        m_V[y][x].m_PX = px;
        m_V[y][x].m_PY = py;
        if ( m_V[y][x].m_N == ENodeVal::FRESH )
        {
            m_V[y][x].m_N = ENodeVal::OPEN;
            m_Exp++;
            Print ( );
        }

        else
        {
            m_Stop = true;
            m_Exp++;
            for ( auto & i : m_ToCnt )
                i->m_N = ENodeVal::CLOSED;
            m_ToCnt.erase ( m_ToCnt.begin ( ), m_ToCnt.end ( ) );
            return;
        }
        m_ToCnt.emplace_back ( & m_V[y][x] );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::DFS ( void )
{
    m_Exp++;
    DFS_p ( m_S.first, m_S.second, m_S.first, m_S.second );
    DrawPath ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::DFS_p ( int x, int y, int px, int py )
{
    if ( m_V[y][x].m_N == ENodeVal::END )
    {
        m_Stop = true;
        m_V[y][x].m_PX = px;
        m_V[y][x].m_PY = py;
        if ( ! ( m_S.first == m_E.first && m_S.second == m_E.second ) )
            m_Exp++;
        return;
    }

    if ( ( m_V[y][x].m_N != ENodeVal::FRESH && m_V[y][x].m_N != ENodeVal::START ) || m_Stop )
        return;

    if ( m_V[y][x].m_N != ENodeVal::START )
    {
        m_V[y][x].m_N = ENodeVal::OPEN;
        m_Exp++;
    }

    m_V[y][x].m_PX = px;
    m_V[y][x].m_PY = py;

    Print ( );

    DFS_p ( x + 1, y, x, y );
    DFS_p ( x, y + 1, x, y );
    DFS_p ( x - 1, y, x, y );
    DFS_p ( x, y - 1, x, y );
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::BFS ( void )
{
    int tmp_x, tmp_y;
    m_Exp++;
    m_Q.push ( & m_V[m_S.second][m_S.first] );
    BFSExpand ( m_S.first, m_S.second, m_S.first, m_S.second );
    
    while ( ! m_Q.empty ( ) )
    {
        tmp_x = m_Q.front ( )->m_X;
        tmp_y = m_Q.front ( )->m_Y;
        m_Q.pop ( );
        BFSExpand ( tmp_x + 1, tmp_y, tmp_x, tmp_y );
        BFSExpand ( tmp_x, tmp_y + 1, tmp_x, tmp_y );
        BFSExpand ( tmp_x - 1, tmp_y, tmp_x, tmp_y );
        BFSExpand ( tmp_x, tmp_y - 1, tmp_x, tmp_y );
    }
    DrawPath ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::BFSExpand ( int x, int y, int px, int py )
{
    if ( m_V[y][x].m_N == ENodeVal::END )
    {
        if ( ! ( m_S.first == m_E.first && m_S.second == m_E.second ) )
            m_Exp++;
        m_V[y][x].m_PX = px;
        m_V[y][x].m_PY = py;
        while ( ! m_Q.empty ( ) )
            m_Q.pop ( );
        return;
    }
    
    if ( m_V[y][x].m_N == ENodeVal::FRESH )
    {
        m_V[y][x].m_N = ENodeVal::OPEN;
        m_Exp++;
        m_V[y][x].m_PX = px;
        m_V[y][x].m_PY = py;
        Print ( );
        m_Q.push ( & m_V[y][x] );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::GreedySearch ( void )
{
    Heuristics ( );
    int tmp_x, tmp_y;
    m_Exp++;
    m_PQ.push ( & m_V[m_S.second][m_S.first] );
    GreedySearchExpand ( m_S.first, m_S.second, m_S.first, m_S.second );

    while ( ! m_PQ.empty ( ) )
    {
        tmp_x = m_PQ.top ( )->m_X;
        tmp_y = m_PQ.top ( )->m_Y;
        m_PQ.pop ( );
        GreedySearchExpand ( tmp_x + 1, tmp_y, tmp_x, tmp_y );
        GreedySearchExpand ( tmp_x, tmp_y + 1, tmp_x, tmp_y );
        GreedySearchExpand ( tmp_x - 1, tmp_y, tmp_x, tmp_y );
        GreedySearchExpand ( tmp_x, tmp_y - 1, tmp_x, tmp_y );
    }
    DrawPath ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::GreedySearchExpand ( int x, int y, int px, int py )
{
    if ( m_V[y][x].m_N == ENodeVal::END )
    {
        if ( ! ( m_S.first == m_E.first && m_S.second == m_E.second ) )
            m_Exp++;
        m_V[y][x].m_PX = px;
        m_V[y][x].m_PY = py;
        while ( ! m_PQ.empty ( ) )
            m_PQ.pop ( );
    }
    
    if ( m_V[y][x].m_N == ENodeVal::FRESH )
    {
        m_V[y][x].m_N = ENodeVal::OPEN;
        m_Exp++;
        m_V[y][x].m_PX = px;
        m_V[y][x].m_PY = py;
        Print ( );
        m_PQ.push ( & m_V[y][x] );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::Dijkstra ( void )
{
    int tmp_x, tmp_y;
    m_Exp++;
    m_PQ.push ( & m_V[m_S.second][m_S.first] );
    DijkstraExpand ( m_S.first, m_S.second, m_S.first, m_S.second );

    while ( ! m_PQ.empty ( ) )
    {
        tmp_x = m_PQ.top ( )->m_X;
        tmp_y = m_PQ.top ( )->m_Y;
        m_PQ.pop ( );
        DijkstraExpand ( tmp_x + 1, tmp_y, tmp_x, tmp_y );
        DijkstraExpand ( tmp_x, tmp_y + 1, tmp_x, tmp_y );
        DijkstraExpand ( tmp_x - 1, tmp_y, tmp_x, tmp_y );
        DijkstraExpand ( tmp_x, tmp_y - 1, tmp_x, tmp_y );
    }
    DrawPath ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::DijkstraExpand ( int x, int y, int px, int py )
{
    if ( m_V[y][x].m_N == ENodeVal::END )
    {
        if ( ! ( m_S.first == m_E.first && m_S.second == m_E.second ) )
            m_Exp++;
        m_V[y][x].m_PX = px;
        m_V[y][x].m_PY = py;
        if ( m_V[y][x].m_ER > 1 + m_V[py][px].m_ER )
            m_V[y][x].m_ER = 1 + m_V[py][px].m_ER;
        m_V[y][x].m_Tot = m_V[y][x].m_ER;
        while ( ! m_PQ.empty ( ) )
            m_PQ.pop ( );
        return;
    }
    
    if ( m_V[y][x].m_ER > 1 + m_V[py][px].m_ER )
    {
        m_V[y][x].m_ER = 1 + m_V[py][px].m_ER;
        m_V[y][x].m_Tot = m_V[y][x].m_ER;
        if ( m_V[y][x].m_N == ENodeVal::FRESH )
            m_Exp++;
        m_V[y][x].m_N  = ENodeVal::OPEN;
        m_V[y][x].m_PX = px;
        m_V[y][x].m_PY = py;
        Print ( );
        m_PQ.push ( & m_V[y][x] );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::AStar ( void )
{
    Heuristics ( );
    int tmp_x, tmp_y;
    m_Exp++;
    m_PQ.push ( & m_V[m_S.second][m_S.first] );
    AStarExpand ( m_S.first, m_S.second, m_S.first, m_S.second );

    while ( ! m_PQ.empty ( ) )
    {
        tmp_x = m_PQ.top ( )->m_X;
        tmp_y = m_PQ.top ( )->m_Y;
        m_PQ.pop ( );
        AStarExpand ( tmp_x + 1, tmp_y, tmp_x, tmp_y );
        AStarExpand ( tmp_x, tmp_y + 1, tmp_x, tmp_y );
        AStarExpand ( tmp_x - 1, tmp_y, tmp_x, tmp_y );
        AStarExpand ( tmp_x, tmp_y - 1, tmp_x, tmp_y );
    }
    DrawPath ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::AStarExpand ( int x, int y, int px, int py )
{
    if ( m_V[y][x].m_N == ENodeVal::END )
    {
        if ( ! ( m_S.first == m_E.first && m_S.second == m_E.second ) )
            m_Exp++;
        m_V[y][x].m_PX = px;
        m_V[y][x].m_PY = py;
        if ( m_V[y][x].m_Tot > 1 + m_V[py][px].m_Tot )
            m_V[y][x].m_ER = 1 + m_V[py][px].m_ER;
        m_V[y][x].m_Tot = m_V[y][x].m_ED + m_V[y][x].m_ER;
        while ( ! m_PQ.empty ( ) )
            m_PQ.pop ( );
        return;
    }
    
    if ( m_V[y][x].m_Tot > 1 + m_V[py][px].m_Tot )
    {
        m_V[y][x].m_ER = 1 + m_V[py][px].m_ER;
        m_V[y][x].m_Tot = m_V[y][x].m_ED + m_V[y][x].m_ER;
        if ( m_V[y][x].m_N == ENodeVal::FRESH )
            m_Exp++;
        m_V[y][x].m_N  = ENodeVal::OPEN;
        m_V[y][x].m_PX = px;
        m_V[y][x].m_PY = py;
        Print ( );
        m_PQ.push ( & m_V[y][x] );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::Heuristics ( void )
{
    for ( auto & i : m_V )
        for ( auto & j : i )
            if ( j.m_N != ENodeVal::WALL )
            {
                j.m_ED = sqrt ( pow ( j.m_X - m_E.first, 2 ) + pow ( j.m_Y - m_E.second, 2 ) );
                j.m_Tot = j.m_ED + j.m_ER;
            }
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::Explanation ( ostream & os ) const
{
    os << "---------------------------------------" << endl;
    os << "\033[34mS Start\033[0m" << endl;
    os << "\033[35mE End\033[0m" << endl;
    os << "\033[33m# Opened node\033[0m" << endl;
    os << "\033[32mo Path\033[0m" << endl;
    os << "\033[31mX Wall\033[0m" << endl;
    os << "space Fresh node" << endl;
    os << "---------------------------------------" << endl;
    os << "Nodes expanded: " << m_Exp << endl;
    os << "Path length: " << m_Len << endl;
}
//----------------------------------------------------------------------------------------------------------------------
inline void CGraph::Print ( void ) const
{
    cout << "\033[2J\033[1;1H";
    cout << *this;
    SLEEP_FOR ( 0.01s )
}
//----------------------------------------------------------------------------------------------------------------------
void CGraph::DrawPath ( void )
{
    Print ( );
    m_Len = 0;
    int pos_x = m_E.first, pos_y = m_E.second, pred_x = m_V[pos_y][pos_x].m_PX, pred_y = m_V[pos_y][pos_x].m_PY;
    while ( ! ( pred_x == m_S.first && pred_y == m_S.second ) )
    {
        pred_x = m_V[pos_y][pos_x].m_PX;
        pred_y = m_V[pos_y][pos_x].m_PY;
        if ( m_V[pred_y][pred_x].m_N != ENodeVal::START )
            m_V[pred_y][pred_x].m_N = ENodeVal::PATH;
        m_Len++;
        Print ( );

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
ostream & operator << ( ostream & os, const CGraph & x )
{
    for ( const auto & i : x.m_V )
        for ( const auto & j : i )
            switch ( j.m_N )
            {
                case CGraph::ENodeVal::START:
                    os << "\033[34m" << j.m_N << "\033[0m";
                    break;
                case CGraph::ENodeVal::END:
                    os << "\033[35m" << j.m_N << "\033[0m";
                    break;
                case CGraph::ENodeVal::WALL:
                    os << "\033[31m" << j.m_N << "\033[0m";
                    break;
                case CGraph::ENodeVal::OPEN:
                    os << "\033[33m" << j.m_N << "\033[0m";
                    break;
                case CGraph::ENodeVal::PATH:
                    os << "\033[32m" << j.m_N << "\033[0m";
                    break;
                case CGraph::ENodeVal::NEWLINE:
                    os << '\n';
                    break;
                default:
                    os << ' ';
            }
    x.Explanation ( os );
    return os;
}
//======================================================================================================================
int main ( int argc, char * argv [] )
{
    if ( argc != 2 )
    {
        cout << "<usage> ./a.out <random/dfs/bfs/greedy/dijkstra/astar> < <filename>" << endl;
        return 1;
    }

    CGraph graph;
    graph.GetInput ( );
    if ( ! strncmp ( argv[1], "random", 6 ) )
        graph.RandomSearch ( );
    if ( ! strncmp ( argv[1], "dfs", 3 ) )
        graph.DFS ( );
    if ( ! strncmp ( argv[1], "bfs", 3 ) )
        graph.BFS ( );
    if ( ! strncmp ( argv[1], "greedy", 6 ) )
        graph.GreedySearch ( );
    if ( ! strncmp ( argv[1], "dijkstra", 8 ) )
        graph.Dijkstra ( );
    if ( ! strncmp ( argv[1], "astar", 5 ) )
        graph.AStar ( );
    return 0;
}
