#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
    int * arr;
    int   size;
    int   max_index;
}TARRAY;

typedef struct
{
	char          operation;
	int           base;
	long long int lo;
	long long int hi;
}TINPUT;
//======================================================================================================================
int getInput ( TINPUT * input )
{
    int ret = scanf ( " %c%d%lld%lld", & input->operation, & input->base, & input->lo, & input->hi );
    if (  ret != 4
     || ( input->operation != 'l' && input->operation != 'c' )
     || input->base < 2
     || input->base > 36
     || input->lo < 0
     || input->lo > input->hi )
    {
	    if ( ! feof ( stdin ) || ( feof ( stdin ) && ret > 0 ) )
	        printf ( "Nespravny vstup.\n" );
	    return 0;
    }
	return 1;
}
//======================================================================================================================
void displayPalindrome ( long long int * nmb_dec, TARRAY * nmb_base, int * base )
{
    printf ("%lld = ", *nmb_dec );
    if ( * base <= 10 )
        for ( int i = 0; i < nmb_base->size; i++ )
            printf ( "%d", nmb_base->arr[i] );
    else
        for ( int i = 0; i < nmb_base->size; i++ )
            if ( nmb_base->arr[i] <= 9 )
                printf ( "%d", nmb_base->arr[i] );
            else
                printf ( "%c", nmb_base->arr[i]+87 );
    printf ( " (%d)\n", * base );
}
//======================================================================================================================
TARRAY decimalToBase ( long long int number, int * base )
{
    TARRAY result;
    result.size = (number == 0) ? 1 : ceil(logl(number+1)/logl(* base));
    result.max_index = result.size-1;
    result.arr = ( int * ) calloc ( result.size, sizeof ( int ) );
    
    for ( int i = result.max_index; number > 0; i--, number /= * base )
        result.arr[i] = number % * base ;
        
    return result;
}
//======================================================================================================================
void getNumbers ( TINPUT * input )
{
    long long int nmb_dec, left, right, symmetric;

    for ( nmb_dec = input->lo; nmb_dec <= input->hi; nmb_dec += 1 )
    {
        symmetric = 1;
        TARRAY nmb_base = decimalToBase ( nmb_dec, & input->base );
        for ( right = nmb_base.max_index, left = 0;
              left <=(nmb_base.size/2)-1;
              right--, left++ )
        {
            if ( nmb_base.arr[left] != nmb_base.arr[right] )
            {
                symmetric = 0;
                break;
            }
        }
        if ( symmetric )
            displayPalindrome ( & nmb_dec, & nmb_base, & input->base );
        free ( nmb_base.arr );
    }
}
//======================================================================================================================
long long int palindrome ( long long int to_reverse, int nmb_len, const int * base )
{
    long long int front = to_reverse, digit, back = 0;

    /* If the nmb_len is odd, we don't want to repeat the middle digit, so we remove it.
     * Note, that this doesn't affect the front of the number we will be returning.
     * So we remove that redundant digit from numbers to be reversed.
     */
    if ( nmb_len % 2 == 1 )
        to_reverse /= * base;

    while ( to_reverse )
    {
        digit = to_reverse % * base;  // gets the last digit from the first half of
                                      // palindrome, e.g. 1234 -> 4
        to_reverse /= * base;         // removes the last digit from the first half,
                                      // e.g. 1234 -> 123
        back = back * * base + digit; // 0*10+4 = 4 | 4*10+3 = 43 | 43*10+2 = 432 |
                                      // 432*10+1 = 4321
        front *= * base;              // increases "length" of the first half
                                      // (for further adding), e.g. 1234 -> 12340 ->
                                      // 123400 -> 1234000 -> 12340000
    }
    return front + back;              // 12340000+4321 = 12344321 -> palindrome
}
//======================================================================================================================
long long int getCount ( long long int nmb, const int * base )
{
    /** This function counts number of palindromes between 1 and nmb.*/

    /* Inspiration used in this function and in function palindrome:
     * https://codereview.stackexchange.com/questions/146288/calculate-the-number-of-palindrome-numbers-in-the-given-ranges
     */

    long long int tmp, prefix = nmb;
    int nmb_len = 0, prefix_len = 0;

    for ( nmb_len = 0, tmp = nmb; tmp != 0; tmp /= * base )
        nmb_len += 1;

    // First half of a palindrome. Length is rounded up (for palindromes of odd length).
    prefix_len = ( nmb_len + 1 ) / 2;
    for ( int i = 0; i < nmb_len - prefix_len; i++ )
        prefix /= * base;

    /* It the palindrome is greater than nmb, subtract
     * because this palindrome is invalid (not in range).
     */
    if ( palindrome ( prefix, nmb_len, base ) > nmb )
        prefix -= 1;

    /* The number of palindromes less than nmb is close to prefix * 2 (last
     * digit not repeated or last digit repeated). We adjust the count depending
     * on whether the original number has an even or odd number of digits.
     *
     * For even nmb_len, we are missing palindromes with the second half composed
     * by different numbers than those in the first half but of a lower value than
     * nmb and palindromes with the first half higher than the first half of nmb
     * but with the value still less than nmb.
     * We will add base^prefix_len - 1 - prefix to the count.
     *
     * For odd numLength, we have too many palindromes because there are (nmb_len/2)+1
     * digits being repeated. Palindromes with the last digit repeated are not usable ->
     * -> the palindrome will be too long -> only 1 of the 2 palindromes are usable.
     * We will subtract prefix - (base^(prefix_len-1)-1).
     */

    prefix *= 2;
    long long int adjustment;
    if ( nmb_len % 2 == 1 )
    {
        adjustment = pow(* base, prefix_len - 1);
        prefix -= prefix / 2 - adjustment + 1;
    }
    else
    {
        adjustment = pow(* base, prefix_len);
        prefix += adjustment - 1 - prefix / 2;
    }
    return prefix;
}
//======================================================================================================================
int main ( void )
{
    TINPUT input;
    printf ( "Vstupni intervaly:\n" );
    while ( getInput ( & input ) )
    {
        switch ( input.operation )
        {
            case 'l':
                getNumbers ( & input );
                break;
            case 'c':
                printf ( "Celkem: %lld\n", getCount ( input.hi,     & input.base ) -
                                           getCount ( input.lo - 1, & input.base ) );
                break;
        }
    }
	return 0;
}
