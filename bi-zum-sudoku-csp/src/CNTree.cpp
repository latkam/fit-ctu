#include "CNTree.hpp"
#include "CSudoku.hpp"

template class CNTree<CSudoku>;
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
CNTree<T>::CNode::CNode ( const T val ) :
  m_P ( nullptr ),
  m_V ( val ) { }
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
CNTree<T>::CNode::~CNode ( void )
{
  m_P = nullptr;
  for ( auto child : m_Ch )
    delete child;
}
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
typename CNTree<T>::CNode * CNTree<T>::CNode::Clone ( void ) const
{
  CNode * n = new CNode ( m_V );
  for ( auto child : m_Ch )
  {
    CNode * n_ch = child -> Clone ( );
    n_ch -> m_P = n;
    n -> m_Ch . push_back ( n_ch );
  }
  return n;
}
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename CNTree<T>::CNode * CNTree<T>::CNode::AddChild ( const T val )
{
  CNode * n = new CNode ( val );
  n -> m_P = this;
  m_Ch . push_back ( n );
  return n;
}
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
std::vector<typename CNTree<T>::CNode *> & CNTree<T>::CNode::AddChildren ( const std::vector<T> & vals )
{
  for ( const auto & val : vals )
    AddChild ( val );
  return m_Ch;
}
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void CNTree<T>::CNode::Print ( std::ostream & os )
{
  os << m_V << std::endl;
  for ( const auto child : m_Ch )
    child -> Print ( os );
}
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
CNTree<T>::CNTree ( void ) :
  m_Root ( nullptr ),
  m_Cnt  ( 0 ) { }
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
CNTree<T>::CNTree ( const CNTree<T> & src ) :
  m_Root ( src . m_Root ? src . m_Root -> Clone ( ) : nullptr ),
  m_Cnt  ( src . m_Cnt ) { }
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
CNTree<T>::~CNTree ( void )
{
  delete m_Root;
}
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
CNTree<T> & CNTree<T>::operator = ( const CNTree<T> & src )
{
  if ( this != & src )
  {
    delete m_Root;
    if ( src . m_Root )
      m_Root = src . m_Root -> Clone ( );
    else
      m_Root = nullptr;
    m_Cnt = src . m_Cnt;
  }
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool CNTree<T>::SetRoot ( const T val )
{
  if ( m_Root )
    return false;
  m_Root = new CNode ( val );
  m_Root -> m_P = nullptr;
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
typename CNTree<T>::CNode ** CNTree<T>::GetRoot ( void )
{
  return & m_Root;
}
//----------------------------------------------------------------------------------------------------------------------
