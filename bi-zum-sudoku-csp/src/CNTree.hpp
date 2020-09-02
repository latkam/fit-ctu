#pragma once

#include <iostream>
#include <vector>

/**
 * @class CNTree
 * @brief Represents n-ary tree.
 * @tparam T data type of elements stored in the tree.
 */
template<typename T>
class CNTree
{
public:
                           CNTree      ( void );
                           CNTree      ( const CNTree & src );
                          ~CNTree      ( void );
  CNTree                 & operator =  ( const CNTree & src );

  class CNode
  {
  public:
    CNode                * m_P;
    std::vector<CNode *>   m_Ch;
    T                      m_V;

                           CNode       ( const T val );
                          ~CNode       ( void );
    CNode                * Clone       ( void ) const;
    CNode                * AddChild    ( const T val );
    std::vector<CNode *> & AddChildren ( const std::vector<T> & vals );
    void                   Print       ( std::ostream & os );
  };

  bool                     SetRoot     ( const T val );
  CNode **                 GetRoot     ( void );
  friend std::ostream &    operator << ( std::ostream & os, const CNTree & x )
  {
    if ( x . m_Root )
      x . m_Root -> Print ( os );
    return os;
  }

private:
  CNode * m_Root;
  size_t  m_Cnt;
};