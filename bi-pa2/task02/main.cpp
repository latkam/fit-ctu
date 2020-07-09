#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */
//======================================================================================================================
/**
 * @class CCitizen
 * A class representing a citizen for purposes of a taxes database.
 */
class CCitizen
{
public:
                CCitizen          ( const string & name, const string & addr, const string & account = "" );
                CCitizen          ( const string & account );
    /**
     * @return citizen's name
     */
    string      GetName           ( void ) const;

    /**
     * @return citizen's address
     */
    string      GetAddress        ( void ) const;

    /**
     * @return citizen's account number
     */
    string      GetAccount        ( void ) const;

    /**
     * @return sum of citizen's incomes
     */
    int         GetIncomes        ( void ) const;

    /**
     * @return sum of citizen's expenses
     */
    int         GetExpenses       ( void ) const;

    /**
     * Completes an incomplete ID with the account. Assumes that the citizen is already identified by name and address.
     * @param[in] other pointer to the citizen whose account will be used
     */
    void        CompleteIDAccount ( const CCitizen * other );

    /**
     * Notes citizen's income.
     * @param[in] amount amount to be credited to the citizen's account
     */
    void        Income            ( const int amount );

    /**
     * Notes citizen's expense.
     * @param[in] amount amount to be withdrawn from the citizen's account
     */
    void        Expense           ( const int amount );

    /**
     * Binary predicate which compares two citizens by their name.
     * If names are equal, it compares them by their address.
     * @param[in] a Pointer to the first citizen to compare.
     * @param[in] b Pointer to the second citizen to compare.
     * @return true if the first citizen comes before the second one by name or address respectively
     *         false if the first citizen is equal to the second one or comes after him by name or address respectively
     */
    static bool CmpByName         ( const CCitizen * a, const CCitizen * b );

    /**
     * Binary predicate which compares two citizens by their accounts.
     * @param[in] a Pointer to the first citizen to compare.
     * @param[in] b Pointer to the second citizen to compare.
     * @return true if the first account come before the second in a lexicographical order
     *         false if the first account is equal to the second or comes after it in a lexicographical order
     */
    static bool CmpByAcct         ( const CCitizen * a, const CCitizen * b );

private:
    string m_Name; /**< Citizen's name.*/
    string m_Addr; /**< Citizen's address.*/
    string m_Acct; /**< Citizens's account number.*/
    int    m_SInc; /**< Sum of citizen's income. */
    int    m_SExp; /**< Sum of citizen's expenses. */
};
//======================================================================================================================
/**
 * @class CIterator
 * This class gives us a possibility to iterate over records in a database from the beginning to the end.
 * Records will be ordered lexicographically by citizen's names and addresses respectively.
 */
class CIterator
{
public:
           CIterator ( const vector <CCitizen *> & data );
    /**
     * Checks whether the iterator has reached the end of the register.
     * @return true if end of the register has been reached, false if otherwise
     */
    bool   AtEnd   ( void ) const;

    /**
     * Shifts iterator one position forward.
     */
    void   Next    ( void );

    /**
     * @return name of the person currently being examined
     */
    string Name    ( void ) const;

    /**
     * @return address of the person currently being examined
     */
    string Addr    ( void ) const;

    /**
     * @return account number of the person currently being examined
     */
    string Account ( void ) const;

private:
    vector <CCitizen *>           m_Data; /**< Vector of pointers to citizens ordered by name and address. */
    vector <CCitizen *>::iterator m_Iter; /**< Iterator over the vector of data, set to beginning in the constructor. */
};
//======================================================================================================================
/**
 * @class CTaxRegister
 * A database of all citizens recording all their incomes and expenses. The purpose of this database is to better
 * fight against tax evasion.
 * A citizen is added to the database in the moment of birth and removes from the database in the moment of death.
 * The citizen is identified by his/her name, address and account number.
 * The account number is unique throughout the database. Names and addreses can be repeated, but the pair of name and
 * address is unique.
 */
class CTaxRegister
{
public:
             ~CTaxRegister ( void );
    /**
     * Adds record to the database.
     * @param[in] name citizen's name
     * @param[in] addr citizen's address
     * @param[in] account citizen's account number
     * @return true if the record has been successfully added, false otherwise
     */
    bool      Birth        ( const string & name, const string & addr, const string & account );

    /**
     * Removes record from the database.
     * @param[in] name citizen's name
     * @param[in] addr citizen's address
     * @return true if the record has been successfully removed, false otherwise
     */
    bool      Death        ( const string & name, const string & addr );

    /**
     * Records an citizen's income of a specified amount. The citizen is identified by his name and address.
     * @param[in] name citizen's name
     * @param[in] addr citizen's address
     * @param[in] amount amount of income
     * @return true if the citizen exists and the method succeeded, false otherwise
     */
    bool      Income       ( const string & name, const string & addr, int amount );

    /**
    * Records an citizen's income of a specified amount. The citizen is identified by his account number.
    * @param[in] account citizen's account number
    * @param[in] amount amount of income
    * @return true if the citizen exists and the method succeeded, false otherwise
    */
    bool      Income       ( const string & account, int amount );

    /**
     * Records an citizen's expense of a specified amount. The citizen is identified by his name and address.
     * @param[in] name citizen's name
     * @param[in] addr citizen's address
     * @param[in] amount amount of expense
     * @return true if the citizen exists and the method succeeded, false otherwise
     */
    bool      Expense      ( const string & name, const string & addr, int amount );

    /**
     * Records an citizen's expense of a specified amount. The citizen is identified by his account number.
     * @param[in] account citizen's account number
     * @param[in] amount of expense
     * @return true if the citizen exists and the method succeeded, false otherwise
     */
    bool      Expense      ( const string & account, int amount );

    /**
     * Searches for a citizen with given name and address and returns all available information.
     * @param[in] name citizen's name
     * @param[in] addr citizen's address
     * @param[out] account citizen's account number
     * @param[out] sumIncome sum of citizen's incomes
     * @param[out] sumExpense sum of citizen's expenses
     * @return true if the method succeeded, false otherwise
     */
    bool      Audit      ( const string & name,
                           const string & addr,
                           string       & account,
                           int          & sumIncome,
                           int          & sumExpense ) const;

    /**
     * @return an instance of the CIterator class. Returned object gives us a possibility to iterate over records
     * in a database from the beginning to the end. Records will be ordered lexicographically by citizen's names
     * and addresses respectively.
     */
    CIterator ListByName ( void ) const;
    
private:
    vector <CCitizen *> m_ByName; /**< Vector of pointers to citizens ordered by name and address. */
    vector <CCitizen *> m_ByAcct; /**< Vector of pointers to citizens ordered by account number. */

};
//======================================================================================================================
CCitizen::CCitizen ( const string & name, const string & addr, const string & account ) :
    m_Name ( name ),
    m_Addr ( addr ),
    m_Acct ( account ),
    m_SInc ( 0 ),
    m_SExp ( 0 ) { }
//----------------------------------------------------------------------------------------------------------------------
CCitizen::CCitizen ( const string & account ) :
    m_Name ( "" ),
    m_Addr ( "" ),
    m_Acct ( account ),
    m_SInc ( 0 ),
    m_SExp ( 0 ) { }
//----------------------------------------------------------------------------------------------------------------------
string CCitizen::GetName ( void ) const
{
    return m_Name;
}
//----------------------------------------------------------------------------------------------------------------------
string CCitizen::GetAddress ( void ) const
{
    return m_Addr;
}
//----------------------------------------------------------------------------------------------------------------------
string CCitizen::GetAccount ( void ) const
{
    return m_Acct;
}
//----------------------------------------------------------------------------------------------------------------------
int CCitizen::GetIncomes ( void ) const
{
    return m_SInc;
}
//----------------------------------------------------------------------------------------------------------------------
int CCitizen::GetExpenses ( void ) const
{
    return m_SExp;
}
//----------------------------------------------------------------------------------------------------------------------
void CCitizen::CompleteIDAccount ( const CCitizen * other )
{
    m_Acct = other -> m_Acct;
}
//----------------------------------------------------------------------------------------------------------------------
void CCitizen::Income ( const int amount )
{
    m_SInc += amount;
}
//----------------------------------------------------------------------------------------------------------------------
void CCitizen::Expense ( const int amount )
{
    m_SExp += amount;
}
//----------------------------------------------------------------------------------------------------------------------
bool CCitizen::CmpByName ( const CCitizen * a, const CCitizen * b )
{
    int cmpName;
    return ( ( cmpName = a -> m_Name . compare ( b -> m_Name ) ) < 0
          || ( ! cmpName &&  a -> m_Addr . compare ( b -> m_Addr ) < 0 ) );
}
//----------------------------------------------------------------------------------------------------------------------
bool CCitizen::CmpByAcct ( const CCitizen * a, const CCitizen * b )
{
    return a -> m_Acct . compare ( b -> m_Acct ) < 0;
}
//======================================================================================================================
CIterator::CIterator ( const vector <CCitizen *> & data ) :
    m_Data ( data ),
    m_Iter ( m_Data . begin ( ) ) { }
//----------------------------------------------------------------------------------------------------------------------
bool CIterator::AtEnd ( void ) const
{
    return m_Iter == m_Data . end ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CIterator::Next ( void )
{
    if ( ! AtEnd ( ) )
        advance ( m_Iter, 1 );
}
//----------------------------------------------------------------------------------------------------------------------
string CIterator::Name ( void ) const
{
    return (*m_Iter) -> GetName ( );
}
//----------------------------------------------------------------------------------------------------------------------
string CIterator::Addr ( void ) const
{
    return (*m_Iter) -> GetAddress ( );
}
//----------------------------------------------------------------------------------------------------------------------
string CIterator::Account ( void ) const
{
    return (*m_Iter) -> GetAccount ( );
}
//======================================================================================================================
CTaxRegister::~CTaxRegister ( void )
{
    for ( auto & i : m_ByName )
        delete i;
}
//----------------------------------------------------------------------------------------------------------------------
bool CTaxRegister::Birth ( const string & name, const string & addr, const string & account )
{
    CCitizen * citizen = new CCitizen ( name, addr, account );
    if ( binary_search ( m_ByName . begin ( ), m_ByName . end ( ), citizen, CCitizen::CmpByName )
      || binary_search ( m_ByAcct . begin ( ), m_ByAcct . end ( ), citizen, CCitizen::CmpByAcct ) )
    {
        delete citizen;
        return false;
    }
    auto itName = lower_bound ( m_ByName . begin ( ), m_ByName . end ( ), citizen, CCitizen::CmpByName );
    auto itAcct = lower_bound ( m_ByAcct . begin ( ), m_ByAcct . end ( ), citizen, CCitizen::CmpByAcct );
    m_ByName . insert ( itName, citizen );
    m_ByAcct . insert ( itAcct, citizen );
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CTaxRegister::Death ( const string & name, const string & addr )
{
    CCitizen citizen ( name, addr );
    if ( ! binary_search ( m_ByName . begin ( ), m_ByName . end ( ), & citizen, CCitizen::CmpByName ) )
        return false;

    auto itName = lower_bound ( m_ByName . begin ( ), m_ByName . end ( ), & citizen, CCitizen::CmpByName );
    citizen . CompleteIDAccount ( *itName );
    auto itAcct = lower_bound ( m_ByAcct . begin ( ), m_ByAcct . end ( ), & citizen, CCitizen::CmpByAcct );

    delete ( *itName );
    m_ByName . erase ( itName );
    m_ByAcct . erase ( itAcct );
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CTaxRegister::Income ( const string & name, const string & addr, int amount )
{
    CCitizen citizen ( name, addr );
    if ( ! binary_search ( m_ByName . begin ( ), m_ByName . end ( ), & citizen, CCitizen::CmpByName ) )
        return false;
    auto itName = lower_bound ( m_ByName . begin ( ), m_ByName . end ( ), & citizen, CCitizen::CmpByName );
    (*itName) -> Income ( amount );
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CTaxRegister::Income ( const string & account, int amount )
{
    CCitizen citizen ( account );
    if ( ! binary_search ( m_ByAcct . begin ( ), m_ByAcct . end ( ), & citizen, CCitizen::CmpByAcct ) )
        return false;
    auto itAcct = lower_bound ( m_ByAcct . begin ( ), m_ByAcct . end ( ), & citizen, CCitizen::CmpByAcct );
    (*itAcct) -> Income ( amount );
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CTaxRegister::Expense ( const string & name, const string & addr, int amount )
{
    CCitizen citizen ( name, addr );
    if ( ! binary_search ( m_ByName . begin ( ), m_ByName . end ( ), & citizen, CCitizen::CmpByName ) )
        return false;
    auto itName = lower_bound ( m_ByName . begin ( ), m_ByName . end ( ), & citizen, CCitizen::CmpByName );
    (*itName) -> Expense ( amount );
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CTaxRegister::Expense ( const string & account, int amount )
{
    CCitizen citizen ( account );
    if ( ! binary_search ( m_ByAcct . begin ( ), m_ByAcct . end ( ), & citizen, CCitizen::CmpByAcct ) )
        return false;
    auto itAcct = lower_bound ( m_ByAcct . begin ( ), m_ByAcct . end ( ), & citizen, CCitizen::CmpByAcct );
    (*itAcct) -> Expense ( amount );
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CTaxRegister::Audit ( const string & name,
                           const string & addr,
                           string       & account,
                           int          & sumIncome,
                           int          & sumExpense ) const
{
    CCitizen citizen ( name, addr );
    if ( ! binary_search ( m_ByName . begin ( ), m_ByName . end ( ), & citizen, CCitizen::CmpByName ) )
        return false;

    auto itName = lower_bound ( m_ByName . begin ( ), m_ByName . end ( ), & citizen, CCitizen::CmpByName );
    account    = (*itName) -> GetAccount  ( );
    sumIncome  = (*itName) -> GetIncomes  ( );
    sumExpense = (*itName) -> GetExpenses ( );
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
CIterator CTaxRegister::ListByName ( void ) const
{
    return CIterator ( m_ByName );
}
//======================================================================================================================
#ifndef __PROGTEST__
int main ( void )
{

    return 0;
}
#endif /* __PROGTEST__ */
