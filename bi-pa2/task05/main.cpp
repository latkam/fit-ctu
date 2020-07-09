#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#endif /* __PROGTEST */

#define ADDREF   if ( m_Attach ) m_Attach -> AddRef  ( );
#define RELEASE  if ( m_Attach ) m_Attach -> Release ( );

using namespace std;

#ifndef __PROGTEST__
//======================================================================================================================
class CTimeStamp
{
public:
                   CTimeStamp  ( int year, int month, int day, int hour, int minute, int sec ) :
                     m_Year   ( year ),
                     m_Month  ( month ),
                     m_Day    ( day ),
                     m_Hour   ( hour ),
                     m_Minute ( minute ),
                     m_Sec    ( sec ) { }
  //--------------------------------------------------------------------------------------------------------------------
  int              Compare     ( const CTimeStamp & x ) const
  {
    if ( m_Year   != x . m_Year )
      return m_Year   < x . m_Year   ? -1 : 1;
    if ( m_Month  != x . m_Month )
      return m_Month  < x . m_Month  ? -1 : 1;  
    if ( m_Day    != x . m_Day )
      return m_Day    < x . m_Day    ? -1 : 1;
    if ( m_Hour   != x . m_Hour )
      return m_Hour   < x . m_Hour   ? -1 : 1;
    if ( m_Minute != x . m_Minute )
      return m_Minute < x . m_Minute ? -1 : 1;      
    if ( m_Sec != x . m_Sec )
      return m_Sec    < x . m_Sec    ? -1 : 1;
    return 0;  
  }
  //--------------------------------------------------------------------------------------------------------------------
  friend ostream & operator << ( ostream & os, const CTimeStamp & x )
  {
    os << x . m_Year << "-" << setfill ( '0' );
    os <<  setw ( 2 ) << x . m_Month << "-";
    os <<  setw ( 2 ) << x . m_Day << " ";
    os <<  setw ( 2 ) << x . m_Hour << ":";
    os <<  setw ( 2 ) << x . m_Minute << ":";
    os <<  setw ( 2 ) << x . m_Sec;
    return os;
  }

private:
  int m_Year;
  int m_Month;
  int m_Day;
  int m_Hour;
  int m_Minute;
  int m_Sec;
};
//======================================================================================================================
class CMailBody
{
public:
                   CMailBody   ( int size, const char * data ) :
                     m_Size ( size )
  {
    size_t len = strlen ( data );
    m_Data = new char [len + 1];
    m_Data[len] = '\0';
    strncpy ( m_Data, data, len );
  }
  //--------------------------------------------------------------------------------------------------------------------
                   CMailBody   ( const CMailBody & src ) :
                     m_Size ( src . m_Size )
   {
     size_t len = strlen ( src . m_Data );
     m_Data = new char [len + 1];
     m_Data[len] = '\0';
     strncpy ( m_Data, src . m_Data, len );
   }
  //--------------------------------------------------------------------------------------------------------------------
                  ~CMailBody   ( void )
  {
      delete [] m_Data;
  }
  //--------------------------------------------------------------------------------------------------------------------
  CMailBody      & operator =  ( const CMailBody & src )
  {
    if ( & src == this )
      return *this;

    m_Size = src . m_Size;

    delete [] m_Data;

    size_t len = strlen ( src . m_Data );
    m_Data = new char [len + 1];
    m_Data[len] = '\0';
    strncpy ( m_Data, src . m_Data, len );
    return *this;
  }
  //--------------------------------------------------------------------------------------------------------------------
  friend ostream & operator << ( ostream & os, const CMailBody & x )
  {

    return os << "mail body: " << x . m_Size << " B";
  }

private:
  int    m_Size;
  char * m_Data;
};
//======================================================================================================================
class CAttach
{
public:
       CAttach ( int x ) :
         m_X (x),
         m_RefCnt ( 1 ) { }
  void AddRef  ( void ) const
  {
    m_RefCnt ++;
  }
  void Release ( void ) const
  {
    if ( ! -- m_RefCnt )
      delete this;
  }

private:
                   CAttach     ( const CAttach & x );
                  ~CAttach     ( void ) = default;
  CAttach        & operator =  ( const CAttach & x );
  friend ostream & operator << ( ostream & os, const CAttach & x )
  {
    return os << "attachment: " << x . m_X << " B";
  }

  int         m_X;
  mutable int m_RefCnt;
};
//======================================================================================================================
#endif /* __PROGTEST__, DO NOT remove */
//======================================================================================================================
class CMail
{
public:
                     CMail       ( const CTimeStamp & timeStamp,
                                   const string     & from,
                                   const CMailBody  & body,
                                   const CAttach    * attach );
                     CMail       ( const CMail & src );
                     CMail       ( CMail && src ) noexcept ;
                    ~CMail       ( void );
  CMail            & operator =  ( const CMail & src );
  CMail            & operator =  ( CMail && src ) noexcept;
  const string     & From        ( void ) const;
  const CMailBody  & Body        ( void ) const;
  const CTimeStamp & TimeStamp   ( void ) const;
  const CAttach    * Attachment  ( void ) const;
  bool               operator <  ( const CMail & x ) const;
  friend ostream   & operator << ( ostream & os, const CMail & x );

private:
  CTimeStamp      m_Stamp;
  string          m_From;
  CMailBody       m_Body;
  const CAttach * m_Attach;
};
//======================================================================================================================
class CMailBox
{
public:
               CMailBox  ( void );
  bool         Delivery  ( const CMail & mail );
  bool         NewFolder ( const string & folderName );
  bool         MoveMail  ( const string & fromFolder, const string & toFolder );
  list <CMail> ListMail  ( const string & folderName, const CTimeStamp & from, const CTimeStamp & to ) const;
  set <string> ListAddr  ( const CTimeStamp & from, const CTimeStamp & to ) const;
private:
  map <string, set<CMail>> m_Folders;
};
//======================================================================================================================
CMail::CMail ( const CTimeStamp & timeStamp, const string & from, const CMailBody & body, const CAttach * attach ) :
  m_Stamp  ( timeStamp ),
  m_From   ( from ),
  m_Body   ( body ),
  m_Attach ( attach )
{
  ADDREF;
}
//----------------------------------------------------------------------------------------------------------------------
CMail::CMail ( const CMail & src ) :
  m_Stamp  ( src . m_Stamp ),
  m_From   ( src . m_From ),
  m_Body   ( src . m_Body ),
  m_Attach ( src . m_Attach )
{
  ADDREF;
}
//----------------------------------------------------------------------------------------------------------------------
CMail::CMail ( CMail && src ) noexcept :
  m_Stamp  ( src . m_Stamp ),
  m_From   ( src . m_From ),
  m_Body   ( src . m_Body ),
  m_Attach ( src . m_Attach )
{
  src . m_Attach = nullptr;
}
//----------------------------------------------------------------------------------------------------------------------
CMail::~CMail ( void )
{
  RELEASE
}
//----------------------------------------------------------------------------------------------------------------------
CMail & CMail::operator = ( const CMail & src )
{
  if ( & src == this )
    return *this;

  RELEASE

  m_Stamp  = src . m_Stamp;
  m_From   = src . m_From;
  m_Body   = src . m_Body;
  m_Attach = src . m_Attach;

  ADDREF
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
CMail & CMail::operator = ( CMail && src ) noexcept
{
  if ( & src == this )
    return *this;

  RELEASE

  m_Stamp  = src . m_Stamp;
  m_From   = src . m_From;
  m_Body   = src . m_Body;
  m_Attach = src . m_Attach;

  src . m_Attach = nullptr;
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
const string & CMail::From ( void ) const
{
  return m_From;
}
//----------------------------------------------------------------------------------------------------------------------
const CMailBody & CMail::Body ( void ) const
{
  return m_Body;
}
//----------------------------------------------------------------------------------------------------------------------
const CTimeStamp & CMail::TimeStamp ( void ) const
{
  return m_Stamp;
}
//----------------------------------------------------------------------------------------------------------------------
const CAttach * CMail::Attachment ( void ) const
{
  ADDREF
  return m_Attach;
}
//----------------------------------------------------------------------------------------------------------------------
bool CMail::operator < ( const CMail & x ) const
{
  return  m_Stamp . Compare ( x . m_Stamp ) < 0;
}
//----------------------------------------------------------------------------------------------------------------------
ostream & operator << ( ostream & os, const CMail & x )
{
  os << x . m_Stamp << " ";
  os << x . m_From << " ";
  os << x . m_Body;
  if ( x . m_Attach )
    os << " + " << *(x . m_Attach);
  return os;
}
//======================================================================================================================
CMailBox::CMailBox ( void )
{
  m_Folders . insert ( make_pair ( "inbox", set <CMail> ( ) ) );
}
//----------------------------------------------------------------------------------------------------------------------
bool CMailBox::Delivery ( const CMail & mail )
{
  return m_Folders["inbox"] . insert ( mail ) . second;
}
//----------------------------------------------------------------------------------------------------------------------
bool CMailBox::NewFolder ( const string & folderName )
{
  return m_Folders . insert ( make_pair ( folderName, set <CMail> ( ) ) ) . second;
}
//----------------------------------------------------------------------------------------------------------------------
bool CMailBox::MoveMail ( const string & fromFolder, const string & toFolder )
{
  auto search_from = m_Folders . find ( fromFolder );
  auto search_to   = m_Folders . find ( toFolder );

  if ( search_from == m_Folders . end ( ) || search_to == m_Folders . end ( ) )
    return false;

  m_Folders[toFolder] . merge ( m_Folders[fromFolder] );
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
list <CMail> CMailBox::ListMail ( const string & folderName, const CTimeStamp & from, const CTimeStamp & to ) const
{
  if ( m_Folders . find ( folderName ) == m_Folders . end ( ) )
    return list <CMail> ( );

  auto it_from = m_Folders . at ( folderName ) . lower_bound ( CMail ( from, "", CMailBody ( 0, "" ), nullptr ) );
  auto it_to   = m_Folders . at ( folderName ) . upper_bound ( CMail ( to  , "", CMailBody ( 0, "" ), nullptr ) );
  return list <CMail> ( it_from, it_to );
}
//----------------------------------------------------------------------------------------------------------------------
set <string> CMailBox::ListAddr ( const CTimeStamp & from, const CTimeStamp & to ) const
{
  _Rb_tree_const_iterator <CMail> it_from, it_to;

  set <string> res;
  for ( const auto & folder : m_Folders )
  {
    it_from = folder . second . lower_bound ( CMail ( from, "", CMailBody ( 0, "" ), nullptr ) );
    it_to   = folder . second . upper_bound ( CMail ( to,   "", CMailBody ( 0, "" ), nullptr ) );
    for ( auto it_folder = it_from; it_folder != it_to; ++ it_folder )
      res . insert ( (*it_folder) . From ( ) );
  }
  return res;
}
//======================================================================================================================
#ifndef __PROGTEST__
static string showMail ( const list<CMail> & l )
{
  ostringstream oss;
  for ( const auto & x : l )
    oss << x << endl;
  return oss . str ();
}
//----------------------------------------------------------------------------------------------------------------------
static string showUsers ( const set<string> & s )
{
  ostringstream oss;
  for ( const auto & x : s )
    oss << x << endl;
  return oss . str ();
}
//======================================================================================================================
int main ( void )
{
  
  return 0;
}
#endif /* __PROGTEST__ */
