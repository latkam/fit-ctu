#include <stdlib.h>
#include <openssl/evp.h>
#include <string.h>
//--------------------------------------------------------------------------------------------------------------------
void printTextHexa ( const unsigned char * text )
{
    size_t len = strlen ( ( const char * ) text );
    for ( size_t i = 0; i < len; ++i )
        printf ( "%02x", text[i] );
}
//--------------------------------------------------------------------------------------------------------------------
int main ( void )
{

//    unsigned char ot[1024] = "tajnazpravakteramepravenapadla";  // tajna prvni zprava
    unsigned char ot[1024] = "abcdefghijklmnopqrstuvwxyz0123"; // druha, verejne znama zprava
    unsigned char st[1024];                                      // sifrovane texty
    unsigned char key[EVP_MAX_KEY_LENGTH] = "uniquesecretkey";  // muj tajny klic (16 znaku)
    unsigned char iv[EVP_MAX_IV_LENGTH] = "jinyvector";  // inicializacni vektor
    const char cipherName[] = "RC4";
    const EVP_CIPHER * cipher;

    OpenSSL_add_all_ciphers ( );
    /* sifry i hashe by se nahraly pomoci OpenSSL_add_all_algorithms() */
    cipher = EVP_get_cipherbyname ( cipherName );
    if ( ! cipher )
    {
        printf ( "Sifra %s neexistuje.\n", cipherName );
        exit ( 1 );
    }

    int otLength  = strlen ( ( const char * ) ot );
    int stLength  = 0;
    int tmpLength = 0;

    EVP_CIPHER_CTX ctx; // struktura pro kontext

    printf ( "OT: " );
    printTextHexa ( ot );
    printf ( "\n" );

    /* Sifrovani */
    EVP_EncryptInit ( & ctx, cipher, key, iv );  // nastaveni kontextu pro sifrovani
    EVP_EncryptUpdate ( & ctx, st, & tmpLength, ot, otLength );  // sifrovani ot
    stLength += tmpLength;
    EVP_EncryptFinal ( & ctx, st + stLength, & tmpLength );  // dokonceni (ziskani zbytku z kontextu)
    stLength += tmpLength;

    printf ( "Zasifrovano %d znaku.\n", stLength );

    /* Desifrovani */
    EVP_DecryptInit ( & ctx, cipher, key, iv );  // nastaveni kontextu pro desifrovani
    EVP_DecryptUpdate ( & ctx, ot, & tmpLength, st, stLength );  // desifrovani st
    otLength += tmpLength;
    EVP_DecryptFinal ( & ctx, ot + otLength, & tmpLength );  // dokonceni (ziskani zbytku z kontextu)
    otLength += tmpLength;

    /* Vypsani zasifrovaneho a rozsifrovaneho textu. */
    printf ( "ST: %s\nDT: %s\n", st, ot );
    printf ( "ST hexa: " );
    printTextHexa ( st );
    printf ( "\nDT hexa: " );
    printTextHexa ( ot );
    printf ( "\n" );

    exit(0);
}
