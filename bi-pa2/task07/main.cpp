#ifndef __PROGTEST__
#include <cctype>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <set>
#include <list>
#include <forward_list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <deque>
#include <optional>
#include <variant>
#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

template <typename _Type, typename _Comparator = equal_to<typename _Type::value_type>>
class CSearch
{
public:
            CSearch    ( _Comparator cmp = equal_to<typename _Type::value_type> ( ) );
            CSearch    ( const CSearch & src ) = delete;
  CSearch & operator = ( const CSearch & src ) = delete;
  void      Add        ( int id, const _Type & needle );
  set<int>  Search     ( const _Type & hayHeap ) const;

private:
  size_t    NeedleSize ( const _Type & needle ) const;

  vector<pair<int, _Type>>                                                                  m_Needles;
  function<bool ( const typename _Type::value_type &, const typename _Type::value_type & )> m_Cmp;
};
//========================================================================================================
template <typename _Type, typename _Comparator>
CSearch<_Type, _Comparator>::CSearch ( _Comparator cmp ) :
  m_Cmp ( cmp ) { }
//--------------------------------------------------------------------------------------------------------
template <typename _Type, typename _Comparator>
void CSearch<_Type, _Comparator>::Add ( int id, const _Type & needle )
{
  m_Needles . emplace_back ( pair<int, _Type> ( id, needle ) );
}
//--------------------------------------------------------------------------------------------------------
template<typename _Type, typename _Comparator>
set<int> CSearch<_Type, _Comparator>::Search ( const _Type & hayHeap ) const
{
  set<int> res;
  vector<bool> found;

  for ( const auto & needle : m_Needles )
  {
    /* Searching for empty sequence is undefined */
    if ( needle . second . empty ( ) )
      continue;

    for ( typename _Type::const_iterator it_heap = hayHeap . cbegin ( ); it_heap != hayHeap . cend ( ); ++ it_heap )
    {
      found . clear ( );
      if ( m_Cmp ( *it_heap, *(needle . second . cbegin ( )) ) )
      {
        typename _Type::const_iterator it_heap_loc = it_heap;
        found . emplace_back ( true );

        /* Both it_heap_loc and it_needle will look at the next element and examine remaining elements to be found. */
        ++ it_heap_loc;
        for ( typename _Type::const_iterator it_needle = ++ (needle . second . cbegin ( ));
              it_needle != needle . second . cend ( ) && it_heap_loc != hayHeap . cend ( );
              ++ it_heap_loc, ++ it_needle )
        {
          if ( m_Cmp ( *it_heap_loc, *it_needle ) )
            found . emplace_back ( true );
          else
            break;
        }

        /* All elements from the sequence have been found correctly */
        if ( found . size ( ) == NeedleSize ( needle . second ) )
          res . insert ( needle . first );
        continue;
      }
    }
  }
  return res;
}
//--------------------------------------------------------------------------------------------------------
template<typename _Type, typename _Comparator>
size_t CSearch<_Type, _Comparator>::NeedleSize ( const _Type & needle ) const
{
  size_t size = 0;
  for ( typename _Type::const_iterator it_needle = needle . cbegin ( );
        it_needle != needle . cend ( );
        ++ it_needle, ++ size );
  return size;
}
//========================================================================================================
#ifndef __PROGTEST__
class CharComparator
{
public:
  CharComparator ( bool caseSensitive = true ) :
    m_CaseSensitive ( caseSensitive ) { }
  bool operator ( ) ( const char & a, const char & b ) const
  {
    return m_CaseSensitive ? a == b : toupper ( a ) == toupper ( b );
  }

private:
  bool m_CaseSensitive;
};
//--------------------------------------------------------------------------------------------------------
bool upperCaseCompare ( const char & a, const char & b )
{
  return toupper ( a ) == toupper ( b );
}
//========================================================================================================
int main ( void )
{

  return 0;
}
#endif /* __PROGTEST__ */