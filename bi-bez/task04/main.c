#include <stdio.h>
#include <string.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define CERTIFICATE_FILENAME "certificate.pem"
#define PAGE_FILENAME "page.html"

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

  /* Initialise SSL communication. */
  if ( SSL_connect ( ssl ) <= 0 )
  {
    printf ( "SSL_connect error.\n" );
    FREE
    return 1;
  }

  /* Get certitiface. */
  X509 * certificate = SSL_get_peer_certificate ( ssl );
  if ( ! certificate )
  {
    printf ( "Error while obtaining certificate.\n" );
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

  FREE
  return 0;
}
