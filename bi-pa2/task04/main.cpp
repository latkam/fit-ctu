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
using namespace std;
#endif /* __PROGTEST__ */
//======================================================================================================================
template <typename T>
void Swap ( T & a, T & b )
{
  T tmp = a;
  a = b;
  b = tmp;
}
//======================================================================================================================
template <typename T>
class CVector
{
public:
                   CVector     ( void );
                   CVector     ( const CVector & src );
                  ~CVector     ( void );
  void             push_back   ( const T val );
  void             clear       ( void );
  size_t           size        ( void ) const;
  void             reserve     ( size_t size );
  CVector        & operator =  ( const CVector & src );
  T              & operator [] ( size_t idx ) const;
  template <typename X>
  friend ostream & operator << ( ostream & os, const CVector <X> & x );

private:
  T      * m_Data;
  size_t   m_Capacity;
  size_t   m_Size;
};
//======================================================================================================================
class CTransaction
{
public:
                   CTransaction ( void );
                   CTransaction ( const char * counterParty, unsigned int amount, const char * signature, bool from );
                   CTransaction ( const CTransaction & src );
                  ~CTransaction ( void );
  CTransaction   & operator =   ( CTransaction src );
  friend ostream & operator <<  ( ostream & os, const CTransaction & x );

private:
  struct TTrnscData
  {
                    TTrnscData ( void );
                    TTrnscData ( const char * counterParty, unsigned int amount, const char * signature, bool from );
                   ~TTrnscData ( void );

    const char   * m_CntrP;
    unsigned int   m_Amnt;
    const char   * m_Sgntr;
    bool           m_From;
    int            m_TrnscRefCnt;
  };
  TTrnscData * m_TTrnscData;
};
//======================================================================================================================
class CAccount
{
public:
                   CAccount    ( void );
                   CAccount    ( const char * accID, int initialBalance );
                   CAccount    ( const CAccount & src );
                  ~CAccount    ( void );
  int              Balance     ( void );
  void             Transaction ( const char * counterParty, unsigned int amount, const char * signature, bool from );
  void             TrimAccount ( void );
  CAccount       & operator =  ( CAccount src );
  friend ostream & operator << ( ostream & os, const CAccount & x );

  friend class CBank;

private:
  struct TAcctID
  {
                 TAcctID ( void );
                 TAcctID ( const char * accID );
                ~TAcctID ( void );

    const char * m_ID;
    int          m_TIDRefCnt;
  };
  struct TAcctData
  {
                             TAcctData ( void );
                             TAcctData ( int initialBalance );
                             TAcctData ( const TAcctData & src );

    int                      m_InBal;
    int                      m_Bal;
    CVector <CTransaction>   m_Trnsc;
    int                      m_RefCnt;
  };

  void    Detach      ( void );

  TAcctID   * m_AcctID;
  TAcctData * m_Data;
};
//======================================================================================================================
class CBank
{
public:
             CBank       ( void );
             CBank       ( const CBank & src );
            ~CBank       ( void );
  CBank    & operator =  ( CBank src );
  bool       NewAccount  ( const char * accID, int initialBalance );
  bool       Transaction ( const char * debAccID, const char * credAccID, unsigned int amount, const char * signature );
  bool       TrimAccount ( const char * accID );
  CAccount & Account     ( const char * accID );

private:
  struct TData
  {
                       TData ( void );
                       TData ( const TData & src );

    CVector <CAccount> m_Accts;
    int                m_RefCnt;

  };

  void Detach       ( void );

  TData * m_Data;
};
//======================================================================================================================
template <typename T>
CVector<T>::CVector ( void ) :
  m_Data     ( nullptr ),
  m_Capacity ( 0 ),
  m_Size     ( 0 ) { }
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
CVector<T>::CVector ( const CVector & src ) :
  m_Capacity ( src . m_Capacity ),
  m_Size     ( src . m_Size )
{
  m_Data = new T [m_Capacity];
  for ( size_t i = 0; i < src . m_Size; ++ i )
    m_Data[i] = src . m_Data[i];
}
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
CVector<T>::~CVector ( void )
{
  delete [] m_Data;
  m_Data = nullptr;
}
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void CVector<T>::push_back ( const T val )
{
  if ( m_Size == m_Capacity )
    reserve ( ( m_Size * 1.1 ) + 1 ); // !!! In "normal" program, the reallocation coefficient should be 1.5 or 2.
  m_Data[m_Size ++] = val;            // But this program had to be memory-efficient, so I tried not to allocate
}                                     // unnecessary space.
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void CVector<T>::clear ( void )
{
  m_Size = 0;
}
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
inline size_t CVector<T>::size ( void ) const
{
  return m_Size;
}
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void CVector<T>::reserve ( size_t size )
{
  if ( m_Capacity >= size )
    return;
  T * tmp = new T[size];
  for ( size_t i = 0; i < m_Size; ++ i )
    tmp[i] = m_Data[i];

  delete [] m_Data;

  m_Data = tmp;
  m_Capacity = size;
}
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
CVector<T> & CVector<T>::operator = ( const CVector & src )
{
  if ( & src == this )
    return *this;

  delete [] m_Data;

  m_Size     = src . m_Size;
  m_Capacity = src . m_Capacity;
  m_Data     = new T [m_Size];
  for ( size_t i = 0; i < m_Size; ++ i )
    m_Data[i] = src . m_Data[i];
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T & CVector<T>::operator [] ( size_t idx ) const
{
  return m_Data[idx];
}
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
ostream & operator << ( ostream & os, const CVector <T> & x )
{
  for ( size_t i = 0; i < x . m_Size; ++ i )
    os << x . m_Data[i] << endl;
  return os;
}
//======================================================================================================================
CTransaction::TTrnscData::TTrnscData ( void ) :
  m_CntrP       ( nullptr ),
  m_Amnt        ( 0 ),
  m_Sgntr       ( nullptr ),
  m_From        ( false ),
  m_TrnscRefCnt ( 1 ) { }
//----------------------------------------------------------------------------------------------------------------------
CTransaction::TTrnscData::TTrnscData ( const char * counterParty,
                                       unsigned int amount,
                                       const char * signature,
                                       bool from ) :
  m_Amnt        ( amount ),
  m_From        ( from ),
  m_TrnscRefCnt ( 1 )
{
  size_t len_CntrP = strlen ( counterParty ), len_Sgntr = strlen ( signature );

  char * tmp_CntrP = new char [len_CntrP + 1];
  char * tmp_Sgntr = new char [len_Sgntr + 1];

  tmp_CntrP[len_CntrP] = tmp_Sgntr[len_Sgntr] = '\0';

  strcpy ( tmp_CntrP, counterParty );
  strcpy ( tmp_Sgntr, signature );

  m_CntrP = tmp_CntrP;
  m_Sgntr = tmp_Sgntr;
}
//----------------------------------------------------------------------------------------------------------------------
CTransaction::TTrnscData::~TTrnscData ( void )
{
  delete [] m_CntrP;
  delete [] m_Sgntr;
}
//----------------------------------------------------------------------------------------------------------------------
CTransaction::CTransaction ( void ) :
  m_TTrnscData ( new TTrnscData ( ) ) { }
//----------------------------------------------------------------------------------------------------------------------
CTransaction::CTransaction ( const char * counterParty, unsigned int amount, const char * signature, bool from ) :
  m_TTrnscData ( new TTrnscData ( counterParty, amount, signature, from ) ) { }
//----------------------------------------------------------------------------------------------------------------------
CTransaction::CTransaction ( const CTransaction & src ) :
  m_TTrnscData ( src . m_TTrnscData )
{
  m_TTrnscData -> m_TrnscRefCnt ++;
}
//----------------------------------------------------------------------------------------------------------------------
CTransaction::~CTransaction ( void )
{
  if ( m_TTrnscData && -- m_TTrnscData -> m_TrnscRefCnt == 0 )
  {
    delete m_TTrnscData;
    m_TTrnscData = nullptr;
  }
}
//----------------------------------------------------------------------------------------------------------------------
CTransaction & CTransaction::operator = ( CTransaction src )
{
  Swap ( m_TTrnscData, src . m_TTrnscData );
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
ostream & operator << ( ostream & os, const CTransaction & x )
{
  x . m_TTrnscData -> m_From ? os << " + " : os << " - ";
  os << x . m_TTrnscData -> m_Amnt << ", ";
  x . m_TTrnscData -> m_From ? os << "from: " : os << "to: ";
  os << x . m_TTrnscData -> m_CntrP <<", ";
  os << "sign: " << x . m_TTrnscData -> m_Sgntr << "\n";
  return os;
}
//======================================================================================================================
CAccount::TAcctID::TAcctID (void ) :
  m_ID        ( nullptr ),
  m_TIDRefCnt ( 1 ) { }
//----------------------------------------------------------------------------------------------------------------------
CAccount::TAcctID::TAcctID (const char * accID ) :
  m_TIDRefCnt ( 1 )
{
  size_t len_accID = strlen ( accID );
  char * tmp_accID = new char [len_accID + 1];
  tmp_accID[len_accID] = '\0';
  strcpy ( tmp_accID, accID );
  m_ID = tmp_accID;
}
//----------------------------------------------------------------------------------------------------------------------
CAccount::TAcctID::~TAcctID ( void )
{
  delete [] m_ID;
}
//----------------------------------------------------------------------------------------------------------------------
CAccount::TAcctData::TAcctData ( void ) :
  m_InBal  ( 0 ),
  m_Bal    ( 0 ),
  m_Trnsc  ( CVector <CTransaction> ( ) ),
  m_RefCnt ( 1 ) { }
//----------------------------------------------------------------------------------------------------------------------
CAccount::TAcctData::TAcctData ( int initialBalance ) :
  m_InBal ( initialBalance ),
  m_Bal   ( initialBalance ),
  m_Trnsc ( CVector <CTransaction> ( ) ),
  m_RefCnt ( 1 ) { }
//----------------------------------------------------------------------------------------------------------------------
CAccount::TAcctData::TAcctData ( const CAccount::TAcctData & src ) :
  m_InBal  ( src . m_InBal ),
  m_Bal    ( src . m_Bal ),
  m_Trnsc  ( src . m_Trnsc ),
  m_RefCnt ( 1 ) { }
//----------------------------------------------------------------------------------------------------------------------
CAccount::CAccount ( void ) :
  m_AcctID ( new TAcctID   ( ) ),
  m_Data   ( new TAcctData ( ) ) { }
//----------------------------------------------------------------------------------------------------------------------
CAccount::CAccount ( const char * accID, int initialBalance ) :
  m_AcctID ( new TAcctID ( accID ) ),
  m_Data   ( new TAcctData ( initialBalance ) ) { }
//--------------------------------------------------------------------------------------,--------------------------------
CAccount::CAccount ( const CAccount & src ) :
  m_AcctID  ( src . m_AcctID ),
  m_Data    ( src . m_Data )
{
  m_AcctID  -> m_TIDRefCnt ++;
  m_Data -> m_RefCnt ++;
}
//----------------------------------------------------------------------------------------------------------------------
CAccount::~CAccount ( void )
{
  if ( m_AcctID && -- m_AcctID -> m_TIDRefCnt == 0 )
  {
    delete m_AcctID;
    m_AcctID = nullptr;
  }

  if ( m_Data && -- m_Data -> m_RefCnt == 0 )
  {
    delete m_Data;
    m_Data = nullptr;
  }
}
//----------------------------------------------------------------------------------------------------------------------
int CAccount::Balance ( void )
{
  return m_Data -> m_Bal;
}
//----------------------------------------------------------------------------------------------------------------------
void CAccount::Transaction ( const char * counterParty, unsigned int amount, const char * signature, bool from )
{
  Detach ( );
  m_Data -> m_Trnsc . push_back ( CTransaction ( counterParty, amount, signature, from ) );
  from ? m_Data -> m_Bal += amount : m_Data -> m_Bal -= amount;
}
//----------------------------------------------------------------------------------------------------------------------
void CAccount::TrimAccount ( void )
{
  Detach ( );
  m_Data -> m_Trnsc . clear ( );
  m_Data -> m_InBal = m_Data -> m_Bal;
}
//----------------------------------------------------------------------------------------------------------------------
CAccount & CAccount::operator = ( CAccount src )
{
  Swap ( m_AcctID, src . m_AcctID );
  Swap ( m_Data, src . m_Data );
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
ostream & operator << ( ostream & os, const CAccount & x )
{
  size_t size = x . m_Data -> m_Trnsc . size ( );

  os << x . m_AcctID -> m_ID << ":\n";
  os << "   " << x . m_Data -> m_InBal << "\n";
  for ( size_t i = 0; i < size; ++i )
    os << x . m_Data -> m_Trnsc[i];
  os << " = " << x . m_Data -> m_Bal << "\n";
  return os;
}
//----------------------------------------------------------------------------------------------------------------------
void CAccount::Detach ( void )
{
  if ( m_Data -> m_RefCnt > 1 )
  {
    /*
     * Block of transactions data is shared among more than one instance of CAccount.
     * Create own transactions that can be modified without affecting the other CAccount instances.
     */

    TAcctData * tmp = new TAcctData ( *m_Data );

    // tmp is a separate transactions block for use in our instance. Disconnect from the source.
    m_Data -> m_RefCnt --;
    m_Data = tmp;
  }
}
//======================================================================================================================
CBank::TData::TData ( void ) :
  m_Accts  ( CVector <CAccount> ( ) ),
  m_RefCnt ( 1 ) { }
//----------------------------------------------------------------------------------------------------------------------
CBank::TData::TData ( const TData & src ) :
  m_Accts  ( src . m_Accts ),
  m_RefCnt ( 1 ) { }
//----------------------------------------------------------------------------------------------------------------------
CBank::CBank ( void ) :
  m_Data ( new TData ( ) ) { }
//----------------------------------------------------------------------------------------------------------------------
CBank::CBank ( const CBank & src ) :
  m_Data ( src . m_Data )
{
  m_Data -> m_RefCnt ++;
}
//----------------------------------------------------------------------------------------------------------------------
CBank::~CBank ( void )
{
  if ( -- m_Data -> m_RefCnt == 0 )
  {
    delete m_Data;
    m_Data = nullptr;
  }
}
//----------------------------------------------------------------------------------------------------------------------
CBank & CBank::operator = ( CBank src )
{
  Swap ( m_Data, src . m_Data );
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
bool CBank::NewAccount ( const char * accID, int initialBalance )
{
  for ( size_t i = 0; i < m_Data -> m_Accts . size ( ); ++ i )
    if ( ! strcmp ( accID, m_Data -> m_Accts[i] . m_AcctID -> m_ID ) )
      return false;

  Detach ( );
  m_Data -> m_Accts . push_back ( CAccount ( accID, initialBalance ) );
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CBank::Transaction ( const char * debAccID, const char * credAccID, unsigned int amount, const char * signature )
{
  // if debAccID == credAccID -> return false
  if ( ! strcmp ( debAccID, credAccID ) )
    return false;

  size_t i = 0, j = 0;
  for ( ; i < m_Data -> m_Accts . size ( ); ++ i )
    if ( ! strcmp ( debAccID, m_Data -> m_Accts[i] . m_AcctID -> m_ID ) )
      break;

  for ( ; j < m_Data -> m_Accts . size ( ); ++ j )
    if ( ! strcmp ( credAccID, m_Data -> m_Accts[j] . m_AcctID -> m_ID ) )
      break;

  // if either debAccID or credAccID doesn't exists -> return false
  if ( i == m_Data -> m_Accts . size ( ) || j == m_Data -> m_Accts . size ( ) )
    return false;

  // detach and add record for debit and credit account
  Detach ( );
  m_Data -> m_Accts[i] . Transaction ( credAccID, amount, signature, false );
  m_Data -> m_Accts[j] . Transaction ( debAccID,  amount, signature, true  );
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CBank::TrimAccount ( const char * accID )
{
  for ( size_t i = 0; i < m_Data -> m_Accts . size ( ); ++i )
    if ( ! strcmp ( accID, m_Data -> m_Accts[i] . m_AcctID -> m_ID ) )
    {
      Detach ( );
      m_Data -> m_Accts[i] . TrimAccount ( );
      return true;
    }
  return false;
}
//----------------------------------------------------------------------------------------------------------------------
CAccount & CBank::Account ( const char * accID )
{
  for ( size_t i = 0; i < m_Data -> m_Accts . size ( ); ++i )
    if ( ! strcmp ( accID, m_Data -> m_Accts[i] . m_AcctID -> m_ID ) )
      return m_Data -> m_Accts[i];
  throw invalid_argument ( "Account with required ID not found." );
}
//----------------------------------------------------------------------------------------------------------------------
void CBank::Detach ( void )
{
  if ( m_Data -> m_RefCnt > 1 )
  {
    /*
     * Block of accounts data is shared among more than one instance of CBank.
     * Create own accouns that can be modified without affecting the other CBank instances.
     */

    TData * tmp = new TData ( *m_Data );

    // tmp is a separate accounts block for use in our instance. Disconnect from the source.
    m_Data -> m_RefCnt --;
    m_Data = tmp;
  }
}
//======================================================================================================================
#ifndef __PROGTEST__
int main ( void )
{
  ostringstream os;
  char accCpy[100], debCpy[100], credCpy[100], signCpy[100];
  CBank x0;
  assert ( x0 . NewAccount ( "123456", 1000 ) );
  assert ( x0 . NewAccount ( "987654", -500 ) );
  assert ( x0 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
  assert ( x0 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
  assert ( x0 . NewAccount ( "111111", 5000 ) );
  assert ( x0 . Transaction ( "111111", "987654", 290, "Okh6e+8rAiuT5=" ) );
  assert ( x0 . Account ( "123456" ). Balance ( ) ==  -2190 );
  assert ( x0 . Account ( "987654" ). Balance ( ) ==  2980 );
  assert ( x0 . Account ( "111111" ). Balance ( ) ==  4710 );
  os . str ( "" );
  os << x0 . Account ( "123456" );
  assert ( ! strcmp ( os . str () . c_str (), "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n" ) );
  os . str ( "" );
  os << x0 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 290, from: 111111, sign: Okh6e+8rAiuT5=\n = 2980\n" ) );
  os . str ( "" );
  os << x0 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 290, to: 987654, sign: Okh6e+8rAiuT5=\n = 4710\n" ) );
  assert ( x0 . TrimAccount ( "987654" ) );
  assert ( x0 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
  os . str ( "" );
  os << x0 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   2980\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 3103\n" ) );

  CBank x2;
  strncpy ( accCpy, "123456", sizeof ( accCpy ) );
  assert ( x2 . NewAccount ( accCpy, 1000 ));
  strncpy ( accCpy, "987654", sizeof ( accCpy ) );
  assert ( x2 . NewAccount ( accCpy, -500 ));
  strncpy ( debCpy, "123456", sizeof ( debCpy ) );
  strncpy ( credCpy, "987654", sizeof ( credCpy ) );
  strncpy ( signCpy, "XAbG5uKz6E=", sizeof ( signCpy ) );
  assert ( x2 . Transaction ( debCpy, credCpy, 300, signCpy ) );
  strncpy ( debCpy, "123456", sizeof ( debCpy ) );
  strncpy ( credCpy, "987654", sizeof ( credCpy ) );
  strncpy ( signCpy, "AbG5uKz6E=", sizeof ( signCpy ) );
  assert ( x2 . Transaction ( debCpy, credCpy, 2890, signCpy ) );
  strncpy ( accCpy, "111111", sizeof ( accCpy ) );
  assert ( x2 . NewAccount ( accCpy, 5000 ));
  strncpy ( debCpy, "111111", sizeof ( debCpy ) );
  strncpy ( credCpy, "987654", sizeof ( credCpy ) );
  strncpy ( signCpy, "Okh6e+8rAiuT5=", sizeof ( signCpy ) );
  assert ( x2 . Transaction ( debCpy, credCpy, 2890, signCpy ) );
  assert ( x2 . Account ( "123456" ). Balance ( ) ==  -2190 );
  assert ( x2 . Account ( "987654" ). Balance ( ) ==  5580 );
  assert ( x2 . Account ( "111111" ). Balance ( ) ==  2110 );
  os . str ( "" );
  os << x2 . Account ( "123456" );
  assert ( ! strcmp ( os . str () . c_str (), "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n" ) );
  os . str ( "" );
  os << x2 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n = 5580\n" ) );
  os . str ( "" );
  os << x2 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n = 2110\n" ) );
  assert ( x2 . TrimAccount ( "987654" ) );
  strncpy ( debCpy, "111111", sizeof ( debCpy ) );
  strncpy ( credCpy, "987654", sizeof ( credCpy ) );
  strncpy ( signCpy, "asdf78wrnASDT3W", sizeof ( signCpy ) );
  assert ( x2 . Transaction ( debCpy, credCpy, 123, signCpy ) );
  os . str ( "" );
  os << x2 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   5580\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n" ) );

  CBank x4;
  assert ( x4 . NewAccount ( "123456", 1000 ) );
  assert ( x4 . NewAccount ( "987654", -500 ) );
  assert ( !x4 . NewAccount ( "123456", 3000 ) );
  assert ( !x4 . Transaction ( "123456", "666", 100, "123nr6dfqkwbv5" ) );
  assert ( !x4 . Transaction ( "666", "123456", 100, "34dGD74JsdfKGH" ) );
  assert ( !x4 . Transaction ( "123456", "123456", 100, "Juaw7Jasdkjb5" ) );
  try
  {
    x4 . Account ( "666" ). Balance ( );
    assert ( "Missing exception !!" == NULL );
  }
  catch ( ... )
  {
  }
  try
  {
    os << x4 . Account ( "666" ). Balance ( );
    assert ( "Missing exception !!" == NULL );
  }
  catch ( ... )
  {
  }
  assert ( !x4 . TrimAccount ( "666" ) );

  CBank x6;
  assert ( x6 . NewAccount ( "123456", 1000 ) );
  assert ( x6 . NewAccount ( "987654", -500 ) );
  assert ( x6 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
  assert ( x6 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
  assert ( x6 . NewAccount ( "111111", 5000 ) );
  assert ( x6 . Transaction ( "111111", "987654", 2890, "Okh6e+8rAiuT5=" ) );
  CBank x7 ( x6 );
  assert ( x6 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
  assert ( x7 . Transaction ( "111111", "987654", 789, "SGDFTYE3sdfsd3W" ) );
  assert ( x6 . NewAccount ( "99999999", 7000 ) );
  assert ( x6 . Transaction ( "111111", "99999999", 3789, "aher5asdVsAD" ) );
  assert ( x6 . TrimAccount ( "111111" ) );
  assert ( x6 . Transaction ( "123456", "111111", 221, "Q23wr234ER==" ) );
  os . str ( "" );
  os << x6 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   -1802\n + 221, from: 123456, sign: Q23wr234ER==\n = -1581\n" ) );
  os . str ( "" );
  os << x6 . Account ( "99999999" );
  assert ( ! strcmp ( os . str () . c_str (), "99999999:\n   7000\n + 3789, from: 111111, sign: aher5asdVsAD\n = 10789\n" ) );
  os . str ( "" );
  os << x6 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n" ) );
  os . str ( "" );
  os << x7 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n" ) );
  try
  {
    os << x7 . Account ( "99999999" ). Balance ( );
    assert ( "Missing exception !!" == NULL );
  }
  catch ( ... )
  {
  }
  os . str ( "" );
  os << x7 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 789, from: 111111, sign: SGDFTYE3sdfsd3W\n = 6369\n" ) );

  CBank x8;
  CBank x9;
  assert ( x8 . NewAccount ( "123456", 1000 ) );
  assert ( x8 . NewAccount ( "987654", -500 ) );
  assert ( x8 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
  assert ( x8 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
  assert ( x8 . NewAccount ( "111111", 5000 ) );
  assert ( x8 . Transaction ( "111111", "987654", 2890, "Okh6e+8rAiuT5=" ) );
  x9 = x8;
  assert ( x8 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
  assert ( x9 . Transaction ( "111111", "987654", 789, "SGDFTYE3sdfsd3W" ) );
  assert ( x8 . NewAccount ( "99999999", 7000 ) );
  assert ( x8 . Transaction ( "111111", "99999999", 3789, "aher5asdVsAD" ) );
  assert ( x8 . TrimAccount ( "111111" ) );
  os . str ( "" );
  os << x8 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   -1802\n = -1802\n" ) );
  os . str ( "" );
  os << x9 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n" ) );
  return 0;
}
#endif /* __PROGTEST__ */
