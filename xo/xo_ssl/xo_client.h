#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 7070
#define SUCCESS 0
#define READ 1
#define WRITE 2
#define PLAYING 3
#define ONLINE 4

#define XO_3X3 1
#define XO_4X4 2

#define WON 1
#define LOST 2
#define DRAW 3

typedef struct XO_Client{
  char name[100];
  int conn_fd;
  int status;
  char read_buff[1024]; 
  int state;
  int move;
  int player;
  int game_type;
  int result;
  BIO *bio;
  SSL_CTX *ctx;
  SSL *ssl;
} XO_Client;  

extern XO_Client xo_client; 

