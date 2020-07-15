// Matej Latka

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#define FREE { EVP_CIPHER_CTX_free ( ctx );\
               fclose ( inFile );\
               fclose ( outFile );\
               free   ( img_in );\
               free   ( img_out );\
               free   ( header );\
               free   ( inFile_name );\
               free   ( outFile_name ); }

/**
 *  BMP Header structure
 *  | Position | Length |             Meaning                |
 *  |    0     |   2    | Magic number (ASCII characters BM) |
 *  |    2     |   4    |  File size (unsigned int)          |
 *  |    6     |   4    |        2x 2B reserved              |
 *  |   10     |   4    |     Beginning of image data        |
 *  |   14     |  ...   |            Format info             |
 *  |   ...    |  ...   |   Colours palette (not mandatory)  |
 *  |   beg    | -> end |           Image data               |
 */

int main ( int argc, char * argv[] )
{
  if ( argc != 3 )
  {
    printf ( "Usage ./encrypt <bmp filename without extension> <ecb/cbc>\n" );
    return 1;
  }

  if ( strlen ( argv[2] ) != 3 )
  {
    printf ( "Usage ./encrypt <bmp filename without extension> <ecb/cbc>\n" );
    return 1;
  }

  size_t inFile_name_len = strlen ( argv[1] );
  size_t i = inFile_name_len, j = inFile_name_len;
  uint32_t inFile_len, beginning, header_len;
  int ct_len = 0, tmp = 0;

  // read filename
  char * inFile_name  = ( char * ) calloc ( inFile_name_len + 5, sizeof ( *inFile_name  ) );
  char * outFile_name = ( char * ) calloc ( inFile_name_len + 9, sizeof ( *outFile_name ) );

  strncpy ( inFile_name,  argv[1], inFile_name_len );
  strncpy ( outFile_name, argv[1], inFile_name_len );

  // append mode <ecb/cbc>
  outFile_name[j++] = '_';
  outFile_name[j++] =  argv[2][0];
  outFile_name[j++] =  argv[2][1];
  outFile_name[j++] =  argv[2][2];

  // append extension
  inFile_name[i++] = outFile_name[j++] = '.';
  inFile_name[i++] = outFile_name[j++] = 'b';
  inFile_name[i++] = outFile_name[j++] = 'm';
  inFile_name[i++] = outFile_name[j++] = 'p';

  FILE * inFile  = fopen ( inFile_name,  "rb" );
  FILE * outFile = fopen ( outFile_name, "wb" );

  // read header
  fseek ( inFile, 2, SEEK_SET );
  fread ( & inFile_len, sizeof ( uint32_t ), 1, inFile ); // read length
  fseek ( inFile, 10, SEEK_SET );
  fread ( & beginning, sizeof ( uint32_t ), 1, inFile );  // read beginning
  inFile_len -= beginning;
  header_len = beginning;
  
  // prepare header, inFile and outFile
  unsigned char * header  = ( unsigned char * ) malloc ( header_len * sizeof ( *header  ) );
  unsigned char * img_in  = ( unsigned char * ) malloc ( inFile_len * sizeof ( *img_in  ) );
  unsigned char * img_out = ( unsigned char * ) malloc ( inFile_len * sizeof ( *img_out ) );

  // copy header
  fseek ( inFile, 0, SEEK_SET );
  
  fread  ( header, sizeof ( *header ), header_len, inFile ); 
  fwrite ( header, sizeof ( *header ), header_len, outFile );
  
  // read inFile
  fseek ( inFile, beginning, SEEK_SET );
  fread ( img_in, sizeof ( unsigned char ), inFile_len, inFile );
  
  unsigned char key[EVP_MAX_KEY_LENGTH] = "randomsecretkey";
  unsigned char iv [EVP_MAX_IV_LENGTH]  = "randominvector"; 

  OpenSSL_add_all_ciphers ( );
  EVP_CIPHER_CTX * ctx = EVP_CIPHER_CTX_new ( );

  if ( ! strncmp ( argv[2],  "ecb", 3 ) )
    EVP_EncryptInit ( ctx, EVP_des_ecb ( ), key, iv );
  else if ( ! strncmp ( argv[2], "cbc", 3 ) )
    EVP_EncryptInit ( ctx, EVP_des_cbc ( ), key, iv );
  else
  {
    printf ( "Usage ./encrypt <bmp filename without extension> <ecb/cbc>\n" );
    FREE
    return 1;
  }
    
  EVP_EncryptUpdate ( ctx, img_out, & ct_len, img_in, inFile_len ); 
  EVP_EncryptFinal  ( ctx, img_out + ct_len, & tmp );
  ct_len += tmp;

  fwrite ( img_out, sizeof ( unsigned char ), ct_len, outFile );

  FREE
  return 0;
}
