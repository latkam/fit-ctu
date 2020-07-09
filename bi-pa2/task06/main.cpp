#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */
//======================================================================================================================
class CContent
{
public:
  virtual                     ~CContent   ( void ) = default;
  CContent                   & Get        ( void );
  size_t                       Height     ( void ) const;
  size_t                       Width      ( void ) const;
  virtual unique_ptr<CContent> Clone      ( void ) const = 0;
  void                         ResetRdr   ( void );

  /**
   * Resizes up the content. Effect is reached only if new_height or new_width are greater than
   * original values.
   * @param new_height new height to be reached
   * @param new_width  new height to be reached
   */
  virtual void                 ResizeUp   ( size_t new_height, size_t new_width ) = 0;

  /**
   * Renders the content.
   * @param output vector of strings to be printed out
   * @param output_idx position in the output vector
   * @return true if rendering can continue false otherwise (everything in the cell has already been rendered)
   */
  virtual void                 Render     ( string & output_line ) = 0;

protected:
  size_t m_H   = 0; /* in some cases can be also interpreted as number of rows */
  size_t m_W   = 0; /* in some cases can be also interpreted as number of cols */
  size_t m_Rdr = 0; /* index of data being rendered */
};
//======================================================================================================================
class CEmpty : public CContent
{
public:
  virtual void                 ResizeUp ( size_t new_height, size_t new_width ) override;
  virtual void                 Render   ( string & output_line ) override;
  virtual unique_ptr<CContent> Clone    ( void ) const override;
};
//======================================================================================================================
class CText : public CContent
{
public:
                               CText    ( const string & str, int align );
  void                         SetText  ( const string & str );
  virtual void                 ResizeUp ( size_t new_height, size_t new_width ) override;
  virtual void                 Render   ( string & output_line ) override;
  virtual unique_ptr<CContent> Clone    ( void ) const override;

  static const constexpr int   ALIGN_LEFT  = 0;
  static const constexpr int   ALIGN_RIGHT = 1;

private:
  vector<string> m_Lines;
  int            m_Align;
};
//======================================================================================================================
class CImage : public CContent
{
public:
                               CImage   ( void ) = default;
  CImage                     & AddRow   ( const string & str );
  virtual void                 ResizeUp ( size_t new_height, size_t new_width ) override;
  virtual void                 Render   ( string & output_line ) override;
  virtual unique_ptr<CContent> Clone    ( void ) const override;

private:
  vector<string> m_ImgLines;
};
//======================================================================================================================
class CTable
{
public:
                   CTable           ( size_t rows, size_t cols );
                   CTable           ( const CTable & x );
  CContent       & GetCell          ( size_t row,  size_t col  ) const;
  void             SetCell          ( size_t row,  size_t col, const CContent & content );
  CTable         & operator =       ( const CTable & x );
  friend ostream & operator <<      ( ostream & os, CTable & x );

private:
  void             Adjust           ( void );
  void             Render           ( void );
  void             GenerateHorDelim ( void );
  vector<vector<unique_ptr<CContent>>> m_Content;      /* [rows;cols] */
  vector<vector<unique_ptr<CContent>>> m_ContentToRdr; /* [rows;cols] */
  string                               m_HorDelim;
  vector<string>                       m_Output;
  bool                                 m_Changed  = false;
  bool                                 m_Rendered = false;
};
//======================================================================================================================
CContent & CContent::Get ( void )
{
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
size_t CContent::Height ( void ) const
{
  return m_H;
}
//----------------------------------------------------------------------------------------------------------------------
size_t CContent::Width ( void ) const
{
  return m_W;
}
//----------------------------------------------------------------------------------------------------------------------
void CContent::ResetRdr ( ) {
  m_Rdr = 0;
}
//======================================================================================================================
unique_ptr<CContent> CEmpty::Clone ( void ) const
{
  return unique_ptr<CContent> ( new CEmpty ( *this ) );
}
//----------------------------------------------------------------------------------------------------------------------
void CEmpty::ResizeUp ( size_t new_height, size_t new_width )
{
  m_H = max ( new_height, m_H );
  m_W = max ( new_width,  m_W );
}
//----------------------------------------------------------------------------------------------------------------------
void CEmpty::Render ( string & output_line )
{
  output_line += string ( m_W, ' ' );
  output_line += '|';
}
//======================================================================================================================
CText::CText ( const string & str, int align ) :
  m_Align ( align )
{
  SetText ( str );
}
//----------------------------------------------------------------------------------------------------------------------
void CText::SetText ( const string & str )
{
  m_Lines . clear ( );

  size_t max_len = 0;
  string to_insert;

  for ( const auto & c : str )
  {
    if ( c == '\n' )
    {
      max_len = max ( to_insert . size ( ), max_len );
      m_Lines . emplace_back ( to_insert );
      to_insert . clear ( );
    }
    else
      to_insert += c;
  }
  max_len = max ( to_insert . size ( ), max_len );
  m_Lines . emplace_back ( to_insert );

  if ( m_Align )
    for ( auto & l : m_Lines )
      l . insert ( 0, max_len - l . size ( ), ' ' );
  else
    for ( auto & l : m_Lines )
      l += string ( max_len - l . size ( ), ' ' );

  m_H = m_Lines . size ( );
  m_W = max_len;
}
//----------------------------------------------------------------------------------------------------------------------
unique_ptr<CContent> CText::Clone ( void ) const
{
  return unique_ptr<CContent> ( new CText ( *this ) );
}
//----------------------------------------------------------------------------------------------------------------------
void CText::ResizeUp ( size_t new_height, size_t new_width )
{
  if ( new_height > m_H )
  {
    for ( size_t i = m_H - 1; i < new_height; ++ i )
      m_Lines . emplace_back ( m_W, ' ' );
    m_H = new_height;
  }

  if ( new_width > m_W )
  {
    for ( auto & l : m_Lines )
      if ( m_Align )
        l . insert ( 0, new_width - m_W, ' ' ); // align right
      else
        l += string ( new_width - m_W, ' ' );   // align left
    m_W = new_width;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CText::Render ( string & output_line )
{
  output_line += m_Lines[m_Rdr ++];
  output_line += '|';
}
//======================================================================================================================
CImage & CImage::AddRow ( const string & str )
{
  m_W = str . size ( );
  m_H ++;
  m_ImgLines . emplace_back ( str );
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
unique_ptr<CContent> CImage::Clone ( void ) const
{
  return unique_ptr<CContent> ( new CImage ( *this ) );
}
//----------------------------------------------------------------------------------------------------------------------
void CImage::ResizeUp ( size_t new_height, size_t new_width )
{
  if ( new_height > m_H )
  {
    size_t to_pad = new_height - m_H, pad_up = to_pad / 2, pad_down = to_pad - pad_up;
    m_ImgLines . insert ( m_ImgLines . begin ( ), pad_up,   string ( m_W, ' ' ) );
    m_ImgLines . insert ( m_ImgLines . end   ( ), pad_down, string ( m_W, ' ' ) );
    m_H = new_height;
  }

  if ( new_width > m_W )
  {
    size_t to_pad = new_width - m_W, pad_left = to_pad / 2, pad_right = to_pad - pad_left;
    for ( auto & l : m_ImgLines )
      l = string ( pad_left, ' ' ) += l += string ( pad_right, ' ' );
    m_W = new_width;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CImage::Render ( string & output_line )
{
  output_line += m_ImgLines[m_Rdr ++];
  output_line += '|';
}
//======================================================================================================================
CTable::CTable ( size_t rows, size_t cols )
{
  m_Content . resize ( rows );
  for ( auto & r : m_Content )
    r . resize ( cols );

  for ( auto & r : m_Content )
    for ( auto & c : r )
      c = unique_ptr<CContent> ( new CEmpty ( ) );

  m_ContentToRdr . resize ( rows );
  for ( auto & r : m_ContentToRdr )
    r . resize ( cols );

  for ( auto & r : m_ContentToRdr )
    for ( auto & c : r )
      c = unique_ptr<CContent> ( new CEmpty ( ) );
}
//----------------------------------------------------------------------------------------------------------------------
CTable::CTable ( const CTable & x ) :
  m_Output   ( x . m_Output ),
  m_Changed  ( x . m_Changed ),
  m_Rendered ( x . m_Rendered )
{
  m_Content . resize ( x . m_Content . size ( ) );
  if ( ! m_Content . empty ( ) )
  {
    for ( auto & r : m_Content )
      r . resize ( x . m_Content[0] . size ( ) );

    for ( size_t r = 0; r < m_Content . size ( ); ++ r )
      for ( size_t c = 0;c < m_Content[0] . size ( ); ++ c )
        m_Content[r][c] = x . m_Content[r][c] -> Clone ( );
  }


  m_ContentToRdr . resize ( x . m_ContentToRdr . size ( ) );
  if ( ! m_ContentToRdr . empty ( ) )
  {
    for ( auto & r : m_ContentToRdr )
      r . resize ( x . m_ContentToRdr[0] . size ( ) );

    for ( size_t r = 0; r < m_ContentToRdr . size ( ); ++ r )
      for ( size_t c = 0;c < m_ContentToRdr[0] . size ( ); ++ c )
        m_ContentToRdr[r][c] = x . m_ContentToRdr[r][c] -> Clone ( );
  }
}
//----------------------------------------------------------------------------------------------------------------------
CContent & CTable::GetCell ( size_t row, size_t col ) const
{
  return m_Content[row][col] -> Get ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CTable::SetCell ( size_t row, size_t col, const CContent & content )
{
  m_Content[row][col] = unique_ptr <CContent> ( content . Clone ( ) );
  m_Changed = true;
  m_Rendered = false;
}
//----------------------------------------------------------------------------------------------------------------------
void CTable::Adjust ( void )
{
  if ( ! m_ContentToRdr . empty ( ) )
  {
    // adjust height -> row by row
    for ( size_t r = 0; r < m_ContentToRdr . size ( ); ++ r )
    {
      size_t max_height = 0;
      for ( size_t c = 0; c < m_ContentToRdr[0] . size ( ); ++ c )
        max_height = max ( max_height, m_ContentToRdr[r][c] -> Height ( ) );   // get max height
      for ( size_t c = 0; c < m_ContentToRdr[0] . size ( ); ++ c )
        m_ContentToRdr[r][c] -> ResizeUp ( max_height, 0 );                    // adjust height
    }

    // adjust width -> column by column
    for ( size_t c = 0; c < m_ContentToRdr[0] . size ( ); ++ c )
    {

      size_t max_width = 0;
      for ( size_t r = 0; r < m_ContentToRdr . size ( ); ++ r )
        max_width = max ( max_width, m_ContentToRdr[r][c] -> Width ( ) );     // get max width

      for ( size_t r = 0; r < m_ContentToRdr . size ( ); ++ r )
        m_ContentToRdr[r][c] -> ResizeUp ( 0, max_width );                    // adjust width
    }
  }
  m_Changed = false;
}
//----------------------------------------------------------------------------------------------------------------------
void CTable::Render ( void )
{
  m_Output . clear ( );
  GenerateHorDelim ( );
  m_Output . emplace_back ( m_HorDelim );

  for ( size_t r = 0; r < m_ContentToRdr . size ( ); ++ r )
  {
    for ( size_t h = 0; h < m_ContentToRdr[r][0] -> Height( ); ++ h )
    {
      m_Output . emplace_back ( "|" );
      for ( auto & c : m_ContentToRdr[r] )
        c -> Render ( m_Output[m_Output . size ( ) - 1] );
    }
    m_Output . emplace_back ( m_HorDelim );
  }

  for ( auto & r : m_ContentToRdr )
    for ( auto & c : r )
      c -> ResetRdr ( );

  m_Rendered = true;
}
//----------------------------------------------------------------------------------------------------------------------
void CTable::GenerateHorDelim ( void )
{
  m_HorDelim . clear ( );
  m_HorDelim += "+";
  if ( ! m_ContentToRdr . empty ( ) )
    for ( size_t c = 0; c < m_ContentToRdr[0] . size ( ); ++ c, m_HorDelim += "+" )
    {
      m_HorDelim += string ( m_ContentToRdr[0][c] -> Width ( ), '-' );
    }
}
//----------------------------------------------------------------------------------------------------------------------
CTable & CTable::operator = ( const CTable & x )
{
  if ( & x == this )
    return *this;

  m_Output   = x . m_Output;
  m_Changed  = x . m_Changed;
  m_Rendered = x . m_Rendered;

  m_Content . resize ( x . m_Content . size ( ) );
  if ( ! m_Content . empty ( ) )
  {
    for ( auto & r : m_Content )
      r . resize ( x . m_Content[0] . size ( ) );

    for ( size_t r = 0; r < m_Content . size ( ); ++ r )
      for ( size_t c = 0; c < m_Content[0] . size ( ); ++ c )
        m_Content[r][c] = x . m_Content[r][c] -> Clone ( );
  }

  m_ContentToRdr . resize ( x . m_ContentToRdr . size ( ) );
  if ( ! m_ContentToRdr . empty ( ) )
  {
    for ( auto & r : m_ContentToRdr )
      r . resize ( x . m_ContentToRdr[0] . size ( ) );

    for ( size_t r = 0; r < m_ContentToRdr . size ( ); ++ r )
      for ( size_t c = 0; c < m_ContentToRdr[0] . size ( ); ++ c )
        m_ContentToRdr[r][c] = x . m_ContentToRdr[r][c] -> Clone ( );
  }
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
ostream & operator << ( ostream & os, CTable & x )
{
  if ( x . m_Changed )
  {
    for ( size_t r = 0; r < x . m_Content . size ( ); ++ r )
      for ( size_t c = 0; c < x . m_Content[0] . size ( ); ++ c )
        x . m_ContentToRdr[r][c] = x . m_Content[r][c] -> Clone ( );
    x . Adjust ( );
  }

  if ( ! x . m_Rendered )
    x . Render ( );

  for ( const auto & o : x . m_Output )
    os << o << endl;
  return os;
}
//======================================================================================================================
#ifndef __PROGTEST__
int main ( void )
{

  return 0;
}
#endif /* __PROGTEST__ */