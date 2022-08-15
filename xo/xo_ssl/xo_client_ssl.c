#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include "xo_client.h"

  SSL_CTX *ctx;
  int ssl_errno;


int xo_init_ssl()
{
  char error[512];
  SSL_library_init() ;
  SSL_load_error_strings();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();

  ctx = SSL_CTX_new( SSLv23_client_method());

  if (ctx == NULL) {
    ERR_error_string(ERR_get_error(), error);
    printf("[%s] \n",error);
    exit(1);
  }
  else {
     printf("success \n ");
  }

  if (!(SSL_CTX_use_certificate_chain_file(ctx, "xo_server.pem"))) //set certificate file
   {
     printf("Couldn't read certificate file\n");
     exit(1);
   }

  if (!(SSL_CTX_use_PrivateKey_file(ctx , "xo_privkey.pem", SSL_FILETYPE_PEM))) //set private key file 
   {
     printf("Couldn't read key file");
     exit(1);
   }

   SSL_CTX_set_cipher_list(ctx, "AES128-SHA:DES-CBC3-SHA");

   xo_client.ssl = SSL_new(ctx);

   if (xo_client.ssl == NULL)
   {
      ERR_error_string(ERR_get_error(), error);
      printf("[%s] \n",error);
      exit(1);
   }

return SUCCESS;
}



