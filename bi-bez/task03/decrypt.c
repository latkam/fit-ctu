// Matej Latka

#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#define FREE { EVP_PKEY_free ( privKey );\
               EVP_CIPHER_CTX_free ( ctx );\
               free   ( inFile_name  );\
               free   ( outFile_name );\
               free   ( cipherName );\
               free   ( encryptedKey );\
               fclose ( inFile );\
               fclose ( outFile );\
               fclose ( privKeyFile ); }

#define BLOCK_LEN 16
#define BUFFER_LEN 1024

int main ( int argc, char * argv[] )
{
  if ( argc != 3 )
  {
    printf ( "Usage ./decrypt <bin filename without extension> <pem private key file>\n" );
    return 1;
  }

  size_t inFile_name_len = strlen ( argv[1] );
  size_t i = inFile_name_len, j = inFile_name_len;

  char * inFile_name  = ( char * ) calloc ( inFile_name_len + 9, sizeof ( *inFile_name  ) );
  char * outFile_name = ( char * ) calloc ( inFile_name_len + 9, sizeof ( *outFile_name ) );

  strncpy ( inFile_name,  argv[1], inFile_name_len );
  strncpy ( outFile_name, argv[1], inFile_name_len );

  // append _enc
  inFile_name[i ++] = '_';
  inFile_name[i ++] = 'e';
  inFile_name[i ++] = 'n';
  inFile_name[i ++] = 'c';

  // append _dec
  outFile_name[j ++] = '_';
  outFile_name[j ++] = 'd';
  outFile_name[j ++] = 'e';
  outFile_name[j ++] = 'c';    

  // append extension
  inFile_name[i ++] = outFile_name[j ++] = '.';
  inFile_name[i ++] = outFile_name[j ++] = 'b';
  inFile_name[i ++] = outFile_name[j ++] = 'i';
  inFile_name[i ++] = outFile_name[j ++] = 'n';

  FILE * inFile  = fopen ( inFile_name,  "rb" );
  FILE * outFile = fopen ( outFile_name, "wb" );

  if ( ! inFile || ! outFile )
  {
      printf ( "Cannot open file(s).\n" );
      free   ( inFile_name  );
      free   ( outFile_name );
      return 1;
  }

  // get private key
  FILE * privKeyFile = fopen ( argv[2], "rb" );
  if ( ! privKeyFile )
  {
    printf ( "Cannot open private key file.\n" );
    free   ( inFile_name  );
    free   ( outFile_name );
    fclose ( inFile );
    fclose ( outFile );
    return 1;
  }
  EVP_PKEY * privKey = PEM_read_PrivateKey ( privKeyFile, NULL, NULL, NULL );


  // get cipher type
  OpenSSL_add_all_ciphers ( );
  int cipherName_len;
  fread ( & cipherName_len, sizeof ( int ), 1, inFile );
  char * cipherName = ( char * ) malloc ( cipherName_len * sizeof ( char ) );
  fread ( cipherName, sizeof ( char ), cipherName_len, inFile );
  const EVP_CIPHER * cipher = EVP_get_cipherbyname ( cipherName );

  // get initialisation vector
  unsigned char iv[BLOCK_LEN];
  fread ( iv, sizeof ( unsigned char ), BLOCK_LEN, inFile );  

  // context, encrypted symmetric key
  EVP_CIPHER_CTX * ctx = EVP_CIPHER_CTX_new ( );
  int encryptedKey_len;
  fread ( & encryptedKey_len, sizeof ( int ), 1, inFile );
  unsigned char * encryptedKey = ( unsigned char * ) malloc ( encryptedKey_len * sizeof ( unsigned char ) );
  fread ( encryptedKey, sizeof ( unsigned char ), encryptedKey_len, inFile );


  if ( ! EVP_OpenInit ( ctx, cipher, encryptedKey, encryptedKey_len, iv, privKey ) )
  {
    printf ( "EVP_OpenInit error.\n" );
    FREE
    return 1;
  }

  unsigned char inBuffer[BUFFER_LEN], outBuffer[BUFFER_LEN + EVP_MAX_BLOCK_LENGTH];

  int read = 0, write = 0;

  while ( ( read = fread ( inBuffer, sizeof ( unsigned char ),BUFFER_LEN, inFile ) ) > 0 )
  {
    if ( ! EVP_OpenUpdate ( ctx, outBuffer, & write, inBuffer, read ) )
    {
      printf ( "EVP_OpenUpdate error.\n" );
      FREE
      return 1;
    }

    fwrite ( outBuffer, sizeof ( unsigned char ), write, outFile );
  }

  if ( ! EVP_OpenFinal ( ctx, outBuffer, & write ) )
  {
    printf ( "EVP_OpenFinal error.\n" );
    FREE
    return 1;
  }

  fwrite ( outBuffer, sizeof ( unsigned char ), write, outFile );
  FREE
  return 0;
}