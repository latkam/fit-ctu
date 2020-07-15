#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define CERTIFICATE_FILENAME "certificate.pem"
#define PAGE_FILENAME "page.html"
#define CIPHER_FILENAME "cipher.txt"

# define FREE { SSL_shutdown ( ssl );\
                shutdown ( fd, 2 );\
                SSL_free ( ssl );\
                SSL_CTX_free ( ctx ); }

int main ( void )
{
  /* Initialisation. */
  SSL * ssl = NULL;
  SSL_CTX * ctx = NULL;

  /* Create socket and corresponding primitives. */
  struct sockaddr_in server_addr;
  int fd = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP ); // fd: socket's feature descriptor
  memset ( & server_addr, 0, sizeof ( server_addr ) );
  server_addr . sin_family = AF_INET;
  server_addr . sin_addr . s_addr = inet_addr ( "147.32.232.212" );
  server_addr . sin_port = htons ( 443 );

  if ( connect ( fd, ( struct sockaddr * ) & server_addr, sizeof ( server_addr ) ) != 0 )
  {
    printf ( "Socket connection error.\n" );
    FREE
    return 1;
  }

  /* Initialise OpenSSL library and create context. */
  SSL_library_init ( );
  ctx = SSL_CTX_new ( SSLv23_client_method ( ) );

  /* Ban old protocols. */
  SSL_CTX_set_options ( ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 );

  /* Create SSL structure and assign open connection. */
  ssl = SSL_new ( ctx );
  SSL_set_fd ( ssl, fd );

  /* Specify default locations from which CA certificates should be used. */
  SSL_CTX_set_default_verify_paths ( ctx );

  /*
   * The original cipher was ECDHE-RSA-CHACHA20-POLY1305.
   * 
   * ECDHE:    Diffie-Hellman protocol using eliptic curves.
   * RSA:      Asymmetric cipher.
   * CHACHA20: Stream cipher.
   * POLY1305: Message authentication code function. Uses not only a text, but also its key.
   *           It is used to validate data integrity and message authentication.
   */ 

  /* Extract previously used cipher from the file (if exists). */
  char cipher1[BUFFER_SIZE + 1];
  char cipher1_ban[BUFFER_SIZE + 9] = "DEFAULT:!(NONE)";
  FILE * cipher_file = fopen ( CIPHER_FILENAME, "r" );
  if ( cipher_file )
  {
    fread ( cipher1, sizeof ( char ), BUFFER_SIZE, cipher_file );
    fclose ( cipher_file );
    strncpy ( & cipher1_ban[9], cipher1, strlen ( cipher1 ) );
    printf ( "Will be banned: %s\n", cipher1_ban );
  }
  else
    printf ( "No cipher has already been stored. (NONE) will be banned.\n" );
  
  /* Ban previously used cipher (if exists). */
  SSL_set_cipher_list ( ssl, cipher1_ban );

  /* Initialise SSL communication. */
  if ( SSL_connect ( ssl ) <= 0 )
  {
    printf ( "SSL_connect error.\n" );
    FREE
    return 1;
  }
   
  /* Get certificate. */
  X509 * certificate = SSL_get_peer_certificate ( ssl );
  if ( ! certificate )
  {
    printf ( "Error while obtaining certificate.\n" );
    FREE
    return 1;
  }

  /* Verificate server's certificate. */
  long res = SSL_get_verify_result ( ssl );
  if ( res == X509_V_OK )
    printf ( "Certificate verification succeeded or no peer certificate was presented.\n" );
  else
  {
    printf ( "Certificate verification error. Code: %ld\n", res );
    FREE
    return 1;
  }

  /* Write certificate to the file. */
  FILE * certificate_file = fopen ( CERTIFICATE_FILENAME, "w" );
  PEM_write_X509 ( certificate_file, certificate );
  fclose ( certificate_file );

  /* Print certificate details. */
  BIO * o = BIO_new_fp ( stdout, BIO_CLOSE );
  X509_print_ex ( o, certificate, XN_FLAG_COMPAT, X509_FLAG_COMPAT );

  /* Ask for https://old.fit.cvut.cz/student/odkazy. */
  char buffer[BUFFER_SIZE] = "GET /student/odkazy HTTP/1.1\r\nHost: old.fit.cvut.cz\r\n\r\n";
  if ( SSL_write ( ssl, buffer, strlen ( buffer ) ) <= 0 )
  {
    printf ( "SSL_write error.\n" );
    FREE
    return 1;
  }

  /* Download  https://old.fit.cvut.cz/student/odkazy to the file. */
  int read;
  FILE * page_file = fopen ( PAGE_FILENAME, "w" );
  while ( ( read = SSL_read ( ssl, buffer, BUFFER_SIZE ) ) > 0 )
  {
    printf ( "Downloading page data (%4lu B)...\n", read * sizeof ( char ) );
    fwrite ( buffer, sizeof ( char ), read, page_file );
  }

  fclose ( page_file );

  /* Print and save used cipher name. */
  const char * cipher2 = SSL_CIPHER_get_name ( SSL_get_current_cipher ( ssl ) );
  printf ( "Cipher:\n" );
  printf ( "%s\n\n", cipher2 );

  cipher_file = fopen ( CIPHER_FILENAME, "w" );
  fwrite ( cipher2, sizeof ( char ), strlen ( cipher2 ), cipher_file );
  fclose ( cipher_file );


  /* Print all ciphers available to client. */
  const char * cipher3;
  printf ( "All ciphers available to client:\n" );
  for ( int i = 0; ( cipher3 = SSL_get_cipher_list ( ssl, i ) ); ++ i )
    printf ( "%s\n", cipher3 );

  FREE
  return 0;
}
