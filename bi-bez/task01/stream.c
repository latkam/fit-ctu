#include <stdio.h>

# define TEXT_LEN 30

int main ( void )
{
    unsigned char ot1[TEXT_LEN], st1[2 * TEXT_LEN], st1Trf[TEXT_LEN];
    unsigned char ot2[TEXT_LEN], st2[2 * TEXT_LEN], st2Trf[TEXT_LEN];
    unsigned char heslo[TEXT_LEN];

    scanf ( "%30s", ( char * ) & ot1 );
    scanf ( "%60s", ( char * ) & st1 );
    scanf ( "%60s", ( char * ) & st2 );
    int tmp1, tmp2;

    for( int i = 0; i < 2 * TEXT_LEN; i += 2 )
    {
        ( st1[i] >= '0' && st1[i] <= '9' ) ? ( tmp1 = ( st1[i] - '0' ) * 16 ) : ( tmp1 = ( st1[i] - 'W' ) * 16 );
        ( st1[i + 1] >= '0' && st1[i + 1] <= '9' ) ? ( tmp1 += st1[i + 1] - '0') : ( tmp1 += st1[i + 1] - 'W' );
        ( st2[i] >= '0' && st2[i] <= '9' ) ? ( tmp2 = ( st2[i] - '0' ) * 16 ) : ( tmp2 = ( st2[i] - 'W' ) * 16 );
        ( st2[i + 1] >= '0' && st2[i + 1] <= '9' ) ? ( tmp2 += st2[i + 1] - '0' ) : ( tmp2 += st2[i + 1] - 'W' );
        st1Trf[i/2] = ( char ) tmp1;
        st2Trf[i/2] = ( char ) tmp2;
        heslo [i/2] = ( st1Trf[i/2] ^ ot1   [i/2] );
        ot2   [i/2] = ( heslo [i/2] ^ st2Trf[i/2] );
    }
    printf ( "Vylustena zprava: \"%30s\".\n", ot2 );
    return 0;
}
