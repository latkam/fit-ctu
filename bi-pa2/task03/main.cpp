#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

/**
 * @class CBigInt
 * Class representing large numbers able to perform basic arithmetic operations and comparisions.
 * @note sign is represented by the bool which is true if number <= 0 and false otherwise.
 * @note number 0 has always sign + (see usage of method bool IsZero ( void ) const ).
 */
class CBigInt
{
  public:
                     CBigInt     ( int             nmb = 0 );
                     CBigInt     ( const char    * nmb );
    friend CBigInt   operator +  ( const CBigInt & l, const CBigInt & r );
    friend CBigInt   operator -  ( const CBigInt & l, const CBigInt & r );
    friend CBigInt   operator *  ( const CBigInt & l, const CBigInt & r );
    CBigInt        & operator += ( const CBigInt & other );
    CBigInt        & operator *= ( const CBigInt & other );

    /**
     * Operator <.
     * @param l lvalue to be compared
     * @param r rvalue to be compared
     * @return In trivial cases, when signs differ, returns trivial result.
     *         If signs are same it treats numbers as if they were both >= 0, so for both numbers < 0
     *         calls AbsGreaterThan.
     */
    friend bool      operator <  ( const CBigInt & l,  const CBigInt & r );
    friend bool      operator <= ( const CBigInt & l,  const CBigInt & r );

    /**
     * Operator >.
     * @param l lvalue to be compared
     * @param r rvalue to be compared
     * @return In trivial cases, when signs differ, returns trivial result.
     *         If signs are same it treats numbers as if they were both >= 0, so for both numbers < 0 calls
     *         AbsLessThan.
     */
    friend bool      operator >  ( const CBigInt & l,  const CBigInt & r );
    friend bool      operator >= ( const CBigInt & l,  const CBigInt & r );
    friend bool      operator == ( const CBigInt & l,  const CBigInt & r );
    friend bool      operator != ( const CBigInt & l,  const CBigInt & r );
    friend ostream & operator << ( ostream       & os, const CBigInt & x );
    friend istream & operator >> ( istream       & is,       CBigInt & x );

private:
    bool             IsZero         ( void ) const;
    void             DelLeadZeros   ( size_t i );
    static CBigInt   AbsAdd         ( const CBigInt & l, const CBigInt & r );
    static CBigInt   AbsSub         ( const CBigInt & l, const CBigInt & r );
    static bool      AbsLessThan    ( const CBigInt & l, const CBigInt & r );
    static bool      AbsGreaterThan ( const CBigInt & l, const CBigInt & r );
    bool   m_Sgn;
    string m_Nmb;
};
//======================================================================================================================
CBigInt::CBigInt ( int nmb ) :
    m_Sgn ( nmb < 0 ),
    m_Nmb ( to_string ( abs ( nmb ) ) )
{
    if ( nmb == INT_MIN )
        m_Nmb . erase ( 0, 1 );
}
//----------------------------------------------------------------------------------------------------------------------
CBigInt::CBigInt ( const char * nmb ) :
    m_Sgn ( nmb[0] == '-' ),
    m_Nmb ( nmb )
{
    // first character can only be - or + or number
    if ( nmb[0] != '-' && nmb[0] != '+' && ! isdigit ( nmb[0] ) )
        throw invalid_argument ( "" );

    // check whether the input is correct
    size_t len = strlen ( nmb );
    for ( size_t i = 1; i < len; ++i )
        if ( ! isdigit ( nmb[i] ) )
            throw invalid_argument ( "" );

    DelLeadZeros ( ( nmb[0] == '-' || nmb[0] == '+' ) );
    if ( IsZero ( ) )
        m_Sgn = false;
}
//----------------------------------------------------------------------------------------------------------------------
CBigInt operator + ( const CBigInt & l, const CBigInt & r )
{
    if ( ! l . m_Sgn &&   r . m_Sgn ) // +l, -r
        return CBigInt::AbsSub ( l, r );
    if (   l . m_Sgn && ! r . m_Sgn ) // -l, +r
        return CBigInt::AbsSub ( r, l );

    CBigInt res = CBigInt::AbsAdd ( l, r ); // +, + or -, -
    if ( l . m_Sgn &&   r . m_Sgn ) // -l, -r
        res . m_Sgn = true;
    return res;
}
//----------------------------------------------------------------------------------------------------------------------
CBigInt operator - ( const CBigInt & l, const CBigInt & r )
{
    if ( ! l . m_Sgn && ! r . m_Sgn ) // +l, +r
        return CBigInt::AbsSub ( l, r );
    if ( ! l . m_Sgn && r . m_Sgn )   // +l, -r
        return CBigInt::AbsAdd ( l, r );
    if ( l . m_Sgn && ! r . m_Sgn ) // -l, +r
    {
        CBigInt res = CBigInt::AbsAdd ( l, r );
        res . m_Sgn = true;
        return res;
    }
    return CBigInt::AbsSub ( r, l );  // -l, -r
}
//----------------------------------------------------------------------------------------------------------------------
CBigInt operator * ( const CBigInt & l, const CBigInt & r )
{
    CBigInt res, tmp;
    size_t size_l = l . m_Nmb . size ( ), size_r = r . m_Nmb . size ( );
    tmp . m_Nmb += string ( max ( size_l,  size_r ) - 1, '0' );
    auto it_tmp = tmp . m_Nmb . rbegin ( );
    string to_append;
    int carry = 0;

    for ( auto it_r = r . m_Nmb . rbegin ( ); it_r != r . m_Nmb . rend ( ); ++it_r, to_append += '0' )
    {
        tmp . m_Nmb = string ( max ( size_l,  size_r ), '0' );
        it_tmp = tmp . m_Nmb . rbegin ( );

        for ( auto it_l = l . m_Nmb . rbegin ( ); it_l != l . m_Nmb . rend ( ); ++it_l, ++it_tmp )
        {
            *it_tmp = ( ( (*it_l - '0') * (*it_r - '0') + carry ) % 10 ) + '0';
             carry  =   ( (*it_l - '0') * (*it_r - '0') + carry ) / 10;
        }

        if ( carry )
        {
            if ( it_tmp == tmp . m_Nmb . rend ( ) )
                tmp . m_Nmb . insert ( 0, 1, carry + '0' );
            else
                *it_tmp = carry + '0';
            carry = 0;
        }

        tmp . m_Nmb += to_append; // simulates "shifting" in multiplication
        res += tmp;
    }

    res . DelLeadZeros ( 0 );
    res . m_Sgn = ( l . m_Sgn != r . m_Sgn );
    if ( res . IsZero ( ) )
        res . m_Sgn = false;
    return res;
}
//----------------------------------------------------------------------------------------------------------------------
CBigInt & CBigInt::operator += ( const CBigInt & other )
{
    *this = *this + other;
    return *this;
}
//----------------------------------------------------------------------------------------------------------------------
CBigInt & CBigInt::operator *= ( const CBigInt & other )
{
    *this = *this * other;
    return *this;
}
//----------------------------------------------------------------------------------------------------------------------
bool operator < ( const CBigInt & l, const CBigInt & r )
{
    if ( l . m_Sgn != r . m_Sgn )
        return l .m_Sgn > r . m_Sgn; // different signs -> trivial result
    if ( l . m_Sgn && r . m_Sgn )
        return CBigInt::AbsGreaterThan ( l, r ); // both < 0
    return CBigInt::AbsLessThan ( l, r ); // both >= 0
}
//----------------------------------------------------------------------------------------------------------------------
bool operator <= ( const CBigInt & l, const CBigInt & r )
{
    return ! ( l > r );
}
//----------------------------------------------------------------------------------------------------------------------
bool operator > ( const CBigInt & l, const CBigInt & r )
{
    if ( l . m_Sgn != r . m_Sgn )
        return l . m_Sgn < r . m_Sgn; // different signs -> trivial result
    if ( l . m_Sgn && r . m_Sgn )
        return CBigInt::AbsLessThan ( l, r ); // both < 0
    return CBigInt::AbsGreaterThan ( l, r ); // both >= 0
}
//----------------------------------------------------------------------------------------------------------------------
bool operator >= ( const CBigInt & l, const CBigInt & r )
{
    return ! ( l < r );
}
//----------------------------------------------------------------------------------------------------------------------
bool operator == ( const CBigInt & l, const CBigInt & r )
{
    return ( l . m_Sgn == r . m_Sgn && ! ( l . m_Nmb . compare ( r . m_Nmb ) ) );
    /* return ! ( l < r ) && ! ( r < l ); */
}
//----------------------------------------------------------------------------------------------------------------------
bool operator != ( const CBigInt & l, const CBigInt & r )
{
    return ( ! ( l == r ) );
}
//----------------------------------------------------------------------------------------------------------------------
ostream & operator << ( ostream & os, const CBigInt & x )
{
    if ( x . m_Sgn )
        os << "-";
    os << x . m_Nmb;
    return os;
}
//----------------------------------------------------------------------------------------------------------------------
istream & operator >> ( istream & is, CBigInt & x )
{
    bool loaded  = false;
    bool skipped = false;
    CBigInt tmp;

    // skip leading whitespaces
    while ( isspace ( is . peek ( ) ) )
        is . get ( );

    // first character can only be - or + or number
    char c = is . peek ( );
    if ( c != '-' && c != '+' && ! isdigit ( c ) )
    {
        is.setstate ( ios_base::failbit );
        return is;
    }

    // check for - or +
    if ( c == '-' || c == '+' )
    {
        tmp . m_Sgn = ( c == '-' );
        is . get ( );
    }

    // skip leading zeros
    while ( is . peek ( ) == '0' )
    {
        is . get ( );
        skipped = true;
    }

    // save the last zero if necessary
    if ( ! isdigit ( is . peek ( ) ) && skipped )
    {
        tmp . m_Nmb += '0';
        loaded = true;
    }

    // parse number
    while ( true )
    {
        if ( is . eof ( ) )
            break;

        c = is . get ( );

        if ( ! isdigit ( c ) )
            break;

        tmp . m_Nmb += c;
        loaded = true;
    }

    if ( ! loaded )
    {
        is . setstate ( ios_base::failbit );
        return is;
    }
    x = tmp;
    x . DelLeadZeros ( 0 );
    if ( x . IsZero ( ) )
        x . m_Sgn = false;
    is . clear ( );
    return is;
}
//----------------------------------------------------------------------------------------------------------------------
bool CBigInt::IsZero ( void ) const
{
    for ( const auto & c : m_Nmb )
        if ( c != '0' )
            return false;
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CBigInt::DelLeadZeros ( size_t i )
{
    for ( ; i < m_Nmb . size ( ) - 1; ++i )
        if ( m_Nmb[i] != '0' )
            break;
    m_Nmb . erase ( 0, i );
}
//----------------------------------------------------------------------------------------------------------------------
CBigInt CBigInt::AbsAdd ( const CBigInt & l, const CBigInt & r )
{
    CBigInt res;
    res . m_Nmb += string ( max ( l . m_Nmb . size ( ), r . m_Nmb . size ( ) ) - 1, '0' );
    auto it_l = l . m_Nmb . rbegin ( );
    auto it_r = r . m_Nmb . rbegin ( );
    int carry = 0, tmp = 0;

    for ( auto it_res = res . m_Nmb . rbegin ( ); it_res != res . m_Nmb . rend ( ); ++it_res, tmp = 0 )
    {
        it_l != l . m_Nmb . rend ( ) ? tmp += *it_l - '0' : tmp += 0;
        it_r != r . m_Nmb . rend ( ) ? tmp += *it_r - '0' : tmp += 0;
        tmp += carry;

        *it_res = ( tmp % 10 ) + '0';
        tmp / 10 ? carry = 1 : carry = 0;

        it_l != l . m_Nmb . rend ( ) ? ++it_l : it_l;
        it_r != r . m_Nmb . rend ( ) ? ++it_r : it_r;
    }

    if ( carry )
        res . m_Nmb . insert ( 0, 1, '1' );

    if ( res . IsZero ( ) )
        res . m_Sgn = false;
    return res;
}
//----------------------------------------------------------------------------------------------------------------------
CBigInt CBigInt::AbsSub ( const CBigInt & l, const CBigInt & r )
{
    CBigInt res;
    res . m_Nmb += string ( max ( l . m_Nmb . size ( ), r . m_Nmb . size ( ) ) - 1, '0' );
    auto it_l = l . m_Nmb . rbegin ( );
    auto it_r = r . m_Nmb . rbegin ( );
    int tmp_l, tmp_r, carry = 0;
    bool swp = false;

    // if the second operand is in absolute value greater than the first, swap them and make result -
    if ( CBigInt::AbsGreaterThan ( r, l ) )
    {
        swp = true;
        res . m_Sgn = true;
    }

    for ( auto it_res = res . m_Nmb . rbegin ( ); it_res != res . m_Nmb . rend ( ); ++it_res )
    {
        it_l != l . m_Nmb . rend ( ) ? tmp_l = *it_l - '0' : tmp_l = 0;
        it_r != r . m_Nmb . rend ( ) ? tmp_r = *it_r - '0' : tmp_r = 0;

        if ( swp )
        {
            tmp_l += carry;
            tmp_l == 10 ? carry = 1 : carry = 0;
            if ( tmp_l == 10 )
                tmp_r += 10; // implicit adding
            if ( tmp_r < tmp_l )
            {
                tmp_r += 10;
                carry = 1; // one persists ...
            }
            *it_res = ( ( tmp_r - tmp_l ) % 10 ) + '0';
        }
        else
        {
            tmp_r += carry;
            tmp_r == 10 ? carry = 1 : carry = 0;
            if ( tmp_r == 10 )
                tmp_l += 10; // implicit adding
            if ( tmp_l < tmp_r )
            {
                tmp_l += 10;
                carry = 1; // one persists...
            }
            *it_res = ( ( tmp_l - tmp_r ) % 10 ) + '0';
        }

        it_l != l . m_Nmb . rend ( ) ? ++it_l : it_l;
        it_r != r . m_Nmb . rend ( ) ? ++it_r : it_r;
    }

    res . DelLeadZeros ( 0 );
    if ( res . IsZero ( ) )
        res . m_Sgn = false;
    return res;
}
//----------------------------------------------------------------------------------------------------------------------
bool CBigInt::AbsLessThan ( const CBigInt & l, const CBigInt & r )
{
    char cmp_l, cmp_r;
    bool lower = false, tmp;
    auto it_l = l . m_Nmb . rbegin ( );
    auto it_r = r . m_Nmb . rbegin ( );

    for ( ; it_l != l . m_Nmb . rend ( ) || it_r != r . m_Nmb . rend ( ); )
    {
        if ( it_l != l . m_Nmb . rend ( ) )
        {
            cmp_l = *it_l;
            ++it_l;
        }
        else
            cmp_l = '0';

        if ( it_r != r . m_Nmb . rend ( ) )
        {
            cmp_r = *it_r;
            ++it_r;
        }
        else
            cmp_r = '0';

        tmp = cmp_l < cmp_r;
        if ( ! lower )
            lower = tmp;
        else
            lower = lower && cmp_l <= cmp_r;
    }
    return lower;
}
//----------------------------------------------------------------------------------------------------------------------
bool CBigInt::AbsGreaterThan ( const CBigInt & l, const CBigInt & r )
{
    char cmp_l, cmp_r;
    bool greater = false, tmp;
    auto it_l = l . m_Nmb . rbegin ( );
    auto it_r = r . m_Nmb . rbegin ( );

    for ( ; it_l != l . m_Nmb . rend ( ) || it_r != r . m_Nmb . rend ( ); )
    {
        if ( it_l != l . m_Nmb . rend ( ) )
        {
            cmp_l = *it_l;
            ++it_l;
        }
        else
            cmp_l = '0';

        if ( it_r != r . m_Nmb . rend ( ) )
        {
            cmp_r = *it_r;
            ++it_r;
        }
        else
            cmp_r = '0';

        tmp = cmp_l > cmp_r;
        if ( ! greater )
            greater = tmp;
        else
            greater = greater && cmp_l >= cmp_r;
    }
    return greater;
}
//======================================================================================================================
#ifndef __PROGTEST__
static bool equal ( const CBigInt & x, const char * val )
{
    ostringstream oss;
    oss << x;
    return oss . str () == val;
}
int main ( void )
{

    return 0;
}
#endif /* __PROGTEST__ */
