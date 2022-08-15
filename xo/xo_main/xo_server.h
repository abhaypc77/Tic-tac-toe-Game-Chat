#define SERVER_PORT 7070
#define SUCCESS 0

#define OFFLINE 1
#define ONLINE 2
#define BUSY 3

extern int debug_flag;
 
typedef struct connection {
  int idx;
  int conn_fd;
  char read_buff[1024];
  int total_bytes_read;
} connection;


typedef struct XO_Player {
  char name[50];
  int status;
  int game_type;
  connection *cptr;
  struct XO_Player *peer;  
} XO_Player;


