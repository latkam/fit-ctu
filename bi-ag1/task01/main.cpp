#include <iostream>
#include <queue>

using namespace std;

struct TQITEM
{
    int x;
    int y;
    int steps;
    int time;
};

class CMensa
{
public:
         CMensa     ( const int & n, const int & k );
        ~CMensa     ( void );
    void queueInput ( void );
    int  bfs        ( void );
    void printSteps ( void );
    
private:
    int                 m_n;
    int                 m_k;
    int                 m_result;
    bool             ** m_queue;
    bool             ** m_visited;
    queue < TQITEM >    m_q;
};
//----------------------------------------------------------------------------------------------------------------------
CMensa::CMensa ( const int & n, const int & k )
{
    m_n = n;
    m_k = k;
    
    m_queue = new bool * [2];
    for ( int i = 0; i < 2; i++ )
        m_queue [i] = new bool [m_n + m_k + 1];
    
    queueInput ( );
    
    m_visited = new bool * [2];
    for ( int i = 0; i < 2; i++ )
        m_visited [i] = new bool [m_n + m_k + 1];
    
    for ( int i = 0; i < 2; i++ )
        for ( int j = 0; j < m_n + m_k + 1; j++ )
            m_visited[i][j] = ! m_queue[i][j];
        
    m_result = bfs ( );
}
//----------------------------------------------------------------------------------------------------------------------
CMensa::~CMensa ( void )
{
    for ( int i = 0; i < 2; i++ )
    {
        delete [] m_queue[i];
        delete [] m_visited[i];
    }
    delete [] m_queue;
    delete [] m_visited;
}
//----------------------------------------------------------------------------------------------------------------------
void CMensa::queueInput ( void )
{
    char tmp;
    m_queue[0][0] = m_queue[1][0] = false;
    for ( int i = 1; i < m_n + 1; i++ ) // left queue basic part
    {
        cin >> tmp;
        m_queue[0][i] = ( tmp == '.' );
    }
    for ( int i = m_n + 1; i < m_n + m_k + 1; i++ ) // left queue supplementary part
        m_queue[0][i] = true;
    
    for ( int i = 1; i < m_n + 1; i ++ ) // right queue basic part
    {
        cin >> tmp;
        m_queue[1][i] = ( tmp == '.' );
    }
    for ( int i = m_n + 1; i < m_n + m_k + 1; i++ ) // right queue supplementary part
        m_queue[1][i] = true;
}
//----------------------------------------------------------------------------------------------------------------------
int CMensa::bfs ( void )
{
    TQITEM source = { 0, 1, 0, -1 }; // left queue, position no. 1
    m_q.push ( source );
    m_visited[0][1] = true;
    
    while ( ! m_q.empty ( ) )
    {
        TQITEM position = m_q.front ( );
        m_q.pop ();
        
        if ( position.y > m_n ) // window reached
            return position.steps;
        
        if ( ( position.y + 1 <= m_n + m_k )
           && ( ! m_visited[position.x][position.y + 1] ) ) // moving forwards
        {
            m_q.push ( TQITEM { position.x, position.y + 1, position.steps + 1, position.time + 1 } );
            m_visited[position.x][position.y + 1] = true;
        }
        
        if ( ( position.y + m_k <= m_n + m_k )
           && ( ! m_visited[! position.x][position.y + m_k] ) ) // change queue
        {
            m_q.push ( TQITEM { ! position.x, position.y + m_k, position.steps + 1, position.time + 1 } );
            m_visited[! position.x][position.y + m_k] = true;
        }
        
        if ( ( position.y - 1 >= 1 )
             && ( ! m_visited[position.x][position.y - 1] )
             && ( position.y - 1 > position.time + 1 ) ) // moving backwards
        {
            m_q.push ( TQITEM { position.x, position.y - 1, position.steps + 1, position.time + 1 } );
            m_visited[position.x][position.y - 1] = true;
        }
    }
    return -1;
}
//----------------------------------------------------------------------------------------------------------------------
void CMensa::printSteps ( void ) { cout << m_result << endl; }
//----------------------------------------------------------------------------------------------------------------------
int main ( void )
{
    int n, k;
    cin >> n >> k;
    CMensa mensa ( n, k );
    mensa.printSteps ( );
    return 0;
}
