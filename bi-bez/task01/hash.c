#include <stdio.h>
#include <math.h>
#include <openssl/evp.h>

#define ALPHABET_LEN 26

//--------------------------------------------------------------------------------------------------------------------
void printTextHexa ( const unsigned char * text, size_t len )
{
    for ( size_t i = 0; i < len; ++i )
        printf ( "%02x", text[i] );
}
//--------------------------------------------------------------------------------------------------------------------
int main ( void )
{
    char           hashFunction[] = "sha256";    // zvolena hashovaci funkce ("sha1", "md5" ...)
    EVP_MD_CTX     ctx;                          // struktura kontextu
    const EVP_MD * type;                         // typ pouzite hashovaci funkce
    unsigned char  hash[EVP_MAX_MD_SIZE];        // char pole pro hash - 64 bytu (max pro sha 512)
    size_t         hash_len, found = 0;          // vysledna delka hashe, indikator, zda byl pozadovany hash nalezen


    OpenSSL_add_all_digests ( );                  // Inicializace OpenSSL hash funkci
    type = EVP_get_digestbyname ( hashFunction ); // Zjisteni, jaka hashovaci funkce ma byt pouzita

    /* Pokud predchozi prirazeni vratilo -1, nebyla zadana spravne hashovaci funkce */
    if ( ! type )
    {
        printf( "Hash %s neexistuje.\n", hashFunction );
        exit ( 1 );
    }

    size_t text_len = 4;
    /*
    printf ( "Zadejte delku textu k zahashovani:.\n" );
    if ( ! scanf ( "%zu", & text_len ) || text_len <= 0 )
    {
        printf ( "Nespravny vstup. Delka musi byt alespon 1.\n" );
        return 1;
    }
    */
    unsigned char * text = ( unsigned char * ) malloc ( ( text_len + 1 ) * sizeof ( unsigned char ) );

    for ( size_t i = 0; i < text_len; ++i )
        text[i] = 'a';
    text[text_len] = '\0';

    for ( int i = 0; i < ( pow ( ALPHABET_LEN, text_len ) - 1 ); ++i )
    {
        text[text_len - 1] += 1;
        for ( int j = text_len - 1; j >= 1; --j )
        {
            if ( text[j] == '{' )
            {
                text[j] = 'a';
                text[j - 1] += 1;
            }
            else
                break;
        }

        EVP_DigestInit ( & ctx, type );                                  
        EVP_DigestUpdate ( & ctx, text, text_len + 1 );                  
        EVP_DigestFinal ( & ctx, hash, ( unsigned int * ) & hash_len );  

        if ( hash[0] == 0xAA && hash[1] == 0xBB )
        {
            found = 1;
            printf ( "Hash nalezen. Hexadecimalni text: \"" );
			printTextHexa ( text, text_len );
            printf ( "\"\nHash: \"" );
            printTextHexa ( hash, hash_len );
            printf ( "\"\n" );
            break;
        }
    }

    if ( ! found )
        printf ( "Hash nanalezen.\n" );

    free ( text );
    return 0;
}
