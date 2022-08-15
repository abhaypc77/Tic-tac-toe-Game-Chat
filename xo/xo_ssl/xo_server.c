#include <stdio.h>          
#include <sys/socket.h>    
#include <arpa/inet.h>    
#include <stdlib.h>     
#include <string.h> 
#include <unistd.h>        
#include <fcntl.h>        
#include <errno.h>
#include <sys/epoll.h> 
#include <openssl/bio.h>
#include "xo_server.h"
#include "xo_debug.h"
#include "xo_server_ssl.h"

XO_Player *xo_player = NULL;
int num_player = 0;
int debug_flag = 0;
static char Server_Greeting[] = "WELCOME TO XO_SERVER"; 


int init_xo_server()
{
  



return SUCCESS;
}

/*------------------------------------------------------
Function to create Player Table Entry

-------------------------------------------------------*/
int create_xo_player_table_entry(int num_player)
{
  XODL_SER("Method called.\n"); 
 
  XODL_SER("Num of Players = [%d]\n", num_player);
  xo_player = realloc(xo_player, (num_player + 1) * sizeof(XO_Player) );
   
 
return SUCCESS;
}


/*-----------------------------------------------------
Function to fill Player info into Table

-----------------------------------------------------*/
int input_xo_player_data(connection *cptr)
{
  XODL_SER("Method called.\n"); 
  strcpy(xo_player[cptr->idx].name, cptr->read_buff);
  xo_player[cptr->idx].status = ONLINE;
  xo_player[cptr->idx].cptr = cptr;

  XODL_SER("Adding [ %s ] to server with conn_fd [ %d ]\n", xo_player[cptr->idx].name, xo_player[cptr->idx].cptr->conn_fd);

return SUCCESS;
}


/*--------------------------------------------------
Function To get List of Players Online
 
--------------------------------------------------*/
int get_players_list(connection *cptr)
{
  int i;
  char player_list[1024] = "ONLINE_PLAYERS|";
  char player_data[100];
  XODL_SER("Method called.\n"); 
  for(i=0; i<num_player; i++)
  { 
    sprintf(player_data, "%s [%s]|", xo_player[i].name, xo_player[i].status == ONLINE ? "ONLINE" : "BUSY");
    strcat(player_list, player_data);
    //XODL_SER("i = %d player = %s\n", i, player_list);
  }
  player_list[strlen(player_list) - 1] = 0;
  write_to_client(cptr, player_list);
  
return SUCCESS;
}

/*----------------------------------------------------
Function to start client session with server

----------------------------------------------------*/
int start_xo_client_session(connection *cptr)
{
  XODL_SER("Method called.\n"); 
  create_xo_player_table_entry(num_player);
  write_to_client(cptr, Server_Greeting, strlen(Server_Greeting));
  read_from_client(cptr);
  input_xo_player_data(cptr);

return SUCCESS;
}


/*------------------------------------------------
Function to check status of remote player

------------------------------------------------*/
int check_remote_player_status(connection *cptr)
{
  int remote_player;
  char buff[100];
  XODL_SER("Method called.\n"); 
  remote_player = atoi(cptr->read_buff + 11);
  sprintf(buff, "GAME_REQUEST:%s|GAME_TYPE:%s", xo_player[cptr->idx].name,
          (xo_player[cptr->idx].game_type == 1) ? "3X3" : "4X4");
  if (xo_player[remote_player-1].status == ONLINE){
    xo_player[cptr->idx].status = BUSY;
    xo_player[remote_player-1].status = BUSY;
    xo_player[remote_player-1].peer = &xo_player[cptr->idx];
    xo_player[cptr->idx].peer = &xo_player[remote_player-1];
    write_to_client(xo_player[remote_player-1].cptr, buff, strlen(buff));
  }

return SUCCESS;
}

/*------------------------------------------------------
Function to start Game with remote Player

------------------------------------------------------*/
int start_game(connection *cptr)
{
  XODL_SER("Method called.\n"); 
  char buff[] = "REQUEST_ACCEPTED";
  xo_player[cptr->idx].status = BUSY;
  write_to_client(xo_player[cptr->idx].peer->cptr, buff);
  


return SUCCESS;
}

/*----------------------------------------------------
Function to process players move

----------------------------------------------------*/
int proc_players_move(connection *cptr)
{
  XODL_SER("Method called.\n"); 
  write_to_client(xo_player[cptr->idx].peer->cptr, cptr->read_buff);

return SUCCESS;
}

/*-------------------------------------------------
Function to Game Type for player

--------------------------------------------------*/
int set_game_type(connection *cptr)
{
  XODL_SER("Method called.");
  xo_player[cptr->idx].game_type = atoi(cptr->read_buff + 10); 
  XODL_SER("Game_Type = [%d]", xo_player[cptr->idx].game_type);

return SUCCESS;
}


/*-------------------------------------------------
Function to Process Client Request

--------------------------------------------------*/
int proc_client_req(connection *cptr)
{
  XODL_SER("Method called.\n"); 
  if ( strncmp(cptr->read_buff, "GAME_TYPE:", 10) == 0)
    set_game_type(cptr);
  if ( strncmp(cptr->read_buff, "ONLINE_PLAYERS", 14) == 0)
    get_players_list(cptr);
  else if ( strstr(cptr->read_buff, "START_GAME") )
    check_remote_player_status(cptr);
  else if (strncmp(cptr->read_buff, "REQUEST_ACCEPTED", 16) == 0)
    start_game(cptr);    
  else if (strncmp(cptr->read_buff, "MOVE:", 5) == 0)
    proc_players_move(cptr);
  


return SUCCESS;
}


/*---------------------------------------------------
Function to read buffer from client

----------------------------------------------------*/
int read_from_client(connection *cptr)
{
  int bytes_read;
  char buff[1024], *temp;

  XODL_SER("Method called.\n"); 
 // while(1)
 // {
  //  bytes_read = read(cptr->conn_fd, cptr->read_buff, 1024);
    bytes_read = SSL_read(cptr->ssl, cptr->read_buff, 1024);
    temp = strstr(cptr->read_buff, "\\r\\n");
    cptr->total_bytes_read += bytes_read;
    if (temp != NULL) {
//      continue;
//    }
//    else{
      *temp = 0;
//      break;
    }
  if (bytes_read == 0)
    close(cptr->ssl); 
   
  XODL_SER("Buff read from [%s] = [%s], total_bytes_read = [%d], conn_fd = [%d] \n",xo_player[cptr->idx].name,
            cptr->read_buff, bytes_read, cptr->conn_fd);

  proc_client_req(cptr);

return SUCCESS;
}


/*----------------------------------------------------
Function to write buffer to client

-----------------------------------------------------*/

int write_to_client(connection *cptr, char *buff)
{
  int bytes_written;
  char temp_buff[] = "\\r\\n";
  
  XODL_SER("Method called.\n"); 
  strcat(buff,temp_buff);  
  while(1)
  {
   // bytes_written = write(cptr->conn_fd, buff, strlen(buff));
    bytes_written = SSL_write(cptr->ssl, buff, strlen(buff));
    if (bytes_written < strlen(buff))
      continue;
    else 
      break;
  }

  XODL_SER("Buff written to [%s] = [%s], total_bytes_written = [%d], conn_fd = [%d] \n",xo_player[cptr->idx].name, buff,
            bytes_written, cptr->conn_fd);

return SUCCESS;
}

/*----------------------------------------------------------
Function to parse arguments

-----------------------------------------------------------*/
int parse_arguments(int argc, char *argv[])
{
  char option;

  while((option = getopt(argc, argv, "D:")) != -1)
  {
    switch(option)
    {
      case 'D':
        debug_flag = atoi(optarg); // Debug flag value
        break;

      default:
       XODL_SER("Error: Invalid arguments.");
    }    
  } 
return SUCCESS;
}


/*----------------------------------------------------------
Main Function for XO_Server

-----------------------------------------------------------*/

int main(int argc, char *argv[])
{
  struct sockaddr_in xo_server;
  struct epoll_event ev;
  struct epoll_event events[10];
  int i = 0, j = 0, err, sock_fd, efd, num_events, con_fd, data_read;
  char buff[1024] = "aaaa";
  connection *cptr;
  XODL_SER("Method called.\n");
  parse_arguments(argc, argv);
  memset(&xo_server, 0 ,sizeof(xo_server) );
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd <0 ) {
    printf("Error in creating socket [%s]", strerror(errno)); 
    exit(1);
  }
  XODL_SER("Server socket fd = [%d]\n", sock_fd); 
  efd = epoll_create(10);
  
  xo_server.sin_family = AF_INET;
  xo_server.sin_addr.s_addr = htonl(INADDR_ANY);
  xo_server.sin_port = htons(7070);

  if (bind(sock_fd, (struct sockaddr *)&xo_server, sizeof(xo_server) ) ) {
    printf("Failed to bind to SERVER_PORT [%s]\n", strerror(errno));
    exit(1);
  }

  if (fcntl(sock_fd, F_SETFL, O_NONBLOCK)) {
    printf("Could not make the socket non-blocking\n");
    close(sock_fd);
  } else
      printf("Non blocking socket set");

  listen(sock_fd, 1);
  ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
  ev.data.fd = sock_fd;
  epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &ev);
  
  XODL_SER("Epoll Done. Entering while (1).\n"); 
  while (1)
  { 
    num_events = epoll_wait(efd, events, 10, 10000);
    printf("no of events = %d\n", num_events);
    for (i = 0 ;i < num_events; i++)
    {
      if (sock_fd == events[i].data.fd) {
        if ((con_fd = accept(sock_fd, (struct sockaddr *)NULL, NULL) ) == -1)
          printf("Accept error");
        else
          XODL_SER("New Connection established conn_fd == [%d]\n", con_fd);
          cptr = (struct connection *) malloc(sizeof(connection));
          xo_init_ssl(cptr);  
          cptr->bio = BIO_new_socket(con_fd, BIO_NOCLOSE);
          SSL_set_bio(cptr->ssl, cptr->bio, cptr->bio);
          err = SSL_accept(cptr->ssl);

          cptr->conn_fd = con_fd;
          cptr->idx = num_player++;
          ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
          ev.data.fd = con_fd;
          ev.data.ptr = (void *) cptr;
          epoll_ctl(efd, EPOLL_CTL_ADD, con_fd, &ev);
          start_xo_client_session(cptr);    
      } else {
          XODL_SER("Received Request from client with conn_fd = [%d]\n", events[i].data.fd);
          read_from_client(events[i].data.ptr);
      }

    }
  }  
    
return 0;
}
