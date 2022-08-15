#include <stdio.h>
#include <stdlib.h>
#define MAX_MSG_SIZE 1024

#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>

#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <arpa/inet.h>

int sockfd, newsockfd, portno, clilen;
struct sockaddr_in serv_addr, cli_addr;


SSL_CTX *my_ssl_ctx;
SSL *my_ssl;

void init_ssl()
{

   SSLeay_add_ssl_algorithms();
   SSL_load_error_strings();

   my_ssl_ctx = SSL_CTX_new(SSLv23_client_method()); //initialize ssl context

   if (my_ssl_ctx == NULL)
    {
      fprintf(stderr, "Can't allocate SSL context\n");
      exit(1);
    }

    if(!(SSL_CTX_use_certificate_chain_file(my_ssl_ctx, "key.pem"))) //set certificate file
    {
       printf("Couldn't read certificate file\n");
       exit(1);
    }

    if(!(SSL_CTX_use_PrivateKey_file(my_ssl_ctx , "key.pem", SSL_FILETYPE_PEM))) //set private key file 
    {
      printf("Couldn't read key file");
      exit(1);
    }

    SSL_CTX_set_cipher_list(my_ssl_ctx, "AES128-SHA:DES-CBC3-SHA"); //set cipher lists
    my_ssl = SSL_new(my_ssl_ctx);
}

void set_ssl_con()
{

  SSL_set_fd(my_ssl, sockfd);
  SSL_load_error_strings();
  SSL_set_connect_state(my_ssl);

}

void kk_ssl(char *cmd)
{
  int err;
  int ll;
  char read_buf[1024*8];

  err = SSL_write(my_ssl, cmd, strlen(cmd));
  if(err <= 0){
     printf("Error in sending ssl data over secure channel\n");
     exit(1);
  }

  err = SSL_read(my_ssl, read_buf, 8*1024);  
  printf("cmd=[%s] response = [%s]\n", cmd, read_buf);
  
}


void do_ssl_read_write()
{
  kk_ssl("STARTTLS\r\n");
  
}

void create_socket(int argc, char **argv)
{
  sockfd = socket(AF_INET, SOCK_STREAM, 0);//create socket of tcp type
  if (sockfd < 0)
  {
     perror("ERROR opening socket");
     exit(1);
  }
  /* Initialize socket structure */

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  //serv_addr.sin_port=htons(995);
  serv_addr.sin_port=htons(25);

  if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
  {
        printf("\n inet_pton error occured\n");
        exit(1);
  } 

  if ( connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0 )
  {
    printf("Unable to connect\n");
    exit(1);
  }
}

void kk_tcp(char *cmd)
{
  char read_buf[1024*8];
  int wrt;

  wrt = write(sockfd, cmd, strlen(cmd));
  if(wrt < 0 ){
     printf("got error in writing\n");
     exit(1);
  }
  wrt = read(sockfd, read_buf, 1024*8);
  if(wrt < 0)
  {
    printf("got error in read\n");
    exit(1);
  }
  printf("cmd=[%s] response = [%s]\n", cmd, read_buf);
}

void do_tcp_read_write()
{
  int err;
  char write_buf[1024];
  char read_buf[1024*8];
  int wrt;

  
////

  kk_tcp("HELO cavisson.com\r\n"); 
  kk_tcp("EHLO cavisson.com\r\n");
 // kk_tcp("AUTH LOGIN\r\n");
 // kk_tcp("a2FtbGVzaC5rdW1hckBjYXZpc3Nvbi5jb20K\r\n");
 // kk_tcp("Q2F2aXNzb25cIQo=\r\n");
  kk_tcp("STARTTLS\r\n");
  init_ssl();
  set_ssl_con();

  err = SSL_connect(my_ssl);
  if(err <= 0){
     printf("Error in handshake with server\n");
     exit(1);
  }
 kk_ssl("EHLO cavisson.com\r\n"); 
 kk_ssl("AUTH LOGIN\r\n"); 
 kk_ssl("YWJoYXkuY2hhdWRoYXJ5QGNhdmlzc29uLmNvbQ==\r\n"); 
 kk_ssl("Q2F2aXNzb24h\r\n");
 kk_ssl("QUIT\r\n"); 
//  kk_ssl("mail from:test@velanapps.com\r\nrcpt to: harshit.agarwal@cavisson.com\r\ndata\r\nFrom: kk@gmail.com\r\nSubject: test mail from secure starttsl\r\n.\r\n");

}

int main (int argc, char *argv[]) 
{
//  uint8_t buf[MAX_MSG_SIZE];

   char write_buf[1024] = "USER abhay.chaudhary@cavisson.com\r\n";
//   char write_buf[1024] = "USER mrk1988kausalya@gmail.com\r\n";
   char read_buf[1024*8];

  //initialize ssl ctx and structures
//   init_ssl();
  //
  printf("hiiiiiiiiiiiiii\n");

  create_socket(argc, argv);
  //set_ssl_con();

 /* int err = SSL_connect(my_ssl);
  if(err <= 0){
     printf("Error in handshake with server\n");
     exit(1);
  }*/

  printf("handshake and Connection successfully established\n");
  //connection has made successfully now try to send data over this ssl connection

  //do_ssl_read_write();

  do_tcp_read_write(); 
  return 0;
}
