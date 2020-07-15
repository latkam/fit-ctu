// Matej Latka

#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#define FREE { EVP_PKEY_free ( pubKey );\
               EVP_CIPHER_CTX_free ( ctx );\
               free   ( inFile_name  );\
               free   ( outFile_name );\
               free   ( encryptedKey );\
               fclose ( inFile );\
               fclose ( outFile );\
               fclose ( pubKeyFile ); }

#define BLOCK_LEN 16
#define BUFFER_LEN 1024
#define CIPHERNAME_LEN 12

int main ( int argc, char * argv[] )
{
    if ( argc != 3 )
    {
        printf ( "Usage: ./encrypt <.bin filename without extension> <.pem public key file>");
        return 1;
    }

    size_t inFile_name_len = strlen ( argv[1] );
    size_t i = inFile_name_len, j = inFile_name_len;

    // read filename
    char * inFile_name  = ( char * ) calloc ( inFile_name_len + 5, sizeof ( *inFile_name ) );
    char * outFile_name = ( char * ) calloc ( inFile_name_len + 9, sizeof ( *outFile_name ) );

    strncpy ( inFile_name,  argv[1], inFile_name_len );
    strncpy ( outFile_name, argv[1], inFile_name_len );

    // append _enc
    outFile_name[j ++] = '_';
    outFile_name[j ++] = 'e';
    outFile_name[j ++] = 'n';
    outFile_name[j ++] = 'c';

    // append extension
    inFile_name[i ++] = outFile_name[j ++] = '.'; 
    inFile_name[i ++] = outFile_name[j ++] = 'b'; 
    inFile_name[i ++] = outFile_name[j ++] = 'i'; 
    inFile_name[i ++] = outFile_name[j ++] = 'n';

    // prepare input and output files
    FILE * inFile  = fopen ( inFile_name,  "rb" );
    FILE * outFile = fopen ( outFile_name, "wb" );

    if ( ! inFile || ! outFile )
    {
        printf ( "Cannot open file(s).\n" );
        free   ( inFile_name  );
        free   ( outFile_name );
        return 1;
    }

    // get public key
    FILE * pubKeyFile = fopen ( argv[2], "rb" );
    if ( ! pubKeyFile )
    {
        printf ( "Cannot open public key file.\n" );
        free   ( inFile_name  );
        free   ( outFile_name );
        fclose ( inFile );
        fclose ( outFile );
        return 1;
    }
    EVP_PKEY * pubKey = PEM_read_PUBKEY ( pubKeyFile, NULL, NULL, NULL );

    // get cipher type
    OpenSSL_add_all_ciphers ( );
    int cipherName_len = CIPHERNAME_LEN;
    const char cipherName[CIPHERNAME_LEN] = "aes-256-cbc";
    const EVP_CIPHER * cipher = EVP_get_cipherbyname ( cipherName );

    // seed pseudo random generator
    if ( RAND_load_file ( "/dev/random", 32 ) != 32 )
    {
        printf ( "Random generator seed error.\n" );
        EVP_PKEY_free ( pubKey );
        free   ( inFile_name  );
        free   ( outFile_name );
        fclose ( inFile );
        fclose ( outFile );
        fclose ( pubKeyFile );
        return 1;
    }

    // context, encrypted symmetric key and initialisation vector
    EVP_CIPHER_CTX * ctx = EVP_CIPHER_CTX_new ( );
    unsigned char * encryptedKey = ( unsigned char * ) malloc ( EVP_PKEY_size ( pubKey ) * sizeof ( unsigned char ) );
    int encryptedKey_len;
    unsigned char iv[BLOCK_LEN] = "randominvector";

    if ( ! EVP_SealInit ( ctx, cipher, & encryptedKey, & encryptedKey_len, iv, & pubKey, 1 ) )
    {
      printf ( "EVP_SealInit error.\n" );
      FREE
      return 1;
    }
    fwrite ( & cipherName_len, sizeof ( int ), 1, outFile );
    fwrite ( cipherName, sizeof ( char ), cipherName_len, outFile );
    fwrite ( iv, sizeof ( unsigned char ), BLOCK_LEN, outFile );
    fwrite ( & encryptedKey_len, sizeof ( int ), 1, outFile );
    fwrite ( encryptedKey, sizeof ( unsigned char ), encryptedKey_len, outFile );

    unsigned char inBuffer[BUFFER_LEN], outBuffer[BUFFER_LEN + EVP_MAX_BLOCK_LENGTH];
    int read = 0, write = 0;

    while ( ( read = fread ( inBuffer, sizeof ( unsigned char ), BUFFER_LEN, inFile ) ) > 0 )
    {
      if ( ! EVP_SealUpdate ( ctx, outBuffer, & write, inBuffer, read ) )
      {
        printf ( "EVP_SealUpdate error.\n" );
        FREE
        return 1;
      }
      fwrite ( outBuffer, sizeof ( unsigned char ), write, outFile );
    }

    if ( ! EVP_SealFinal ( ctx, outBuffer, & write ) )
    {
      printf ( "EVP_SealFinal error.\n" );
      FREE
      return 1;
    }

    fwrite ( outBuffer, sizeof ( unsigned char ), write, outFile );
    FREE
    return 0;
}