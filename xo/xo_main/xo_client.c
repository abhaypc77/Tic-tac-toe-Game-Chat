#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <signal.h>
#include "xo_client.h"
#include "game_xo_4.h"
#include "xo_debug.h"

int debug_flag;
XO_Client xo_client;

/*------------------------------------------------------------
Function to create TCP connection with server

------------------------------------------------------------*/
int create_tcp_connection()
{
  struct sockaddr_in client;
  int sock_fd, i, efd, num_events;
  struct epoll_event ev;
  struct epoll_event events[10];
  char *buff;
  int timeout = 0;

  XODL_CLI("Method called.");
  memset(&client, 0 , sizeof(client));
  buff = (char*)malloc(1024);

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("ERROR in opening socket");
    exit(1);
  }
  
  efd = epoll_create(10);  
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  client.sin_port = htons(SERVER_PORT);
 
  if (connect(sock_fd, (struct sockaddr *)&client, sizeof(client)) == -1) {
    perror("ERROR in connect");
    exit(1);
  }

  if (fcntl(sock_fd, F_SETFL, O_NONBLOCK)) {
    printf("Could not make the socket non-blocking\n");
    close(sock_fd);
  } else
      XODL_CLI("Non blocking socket set\n");

  ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
  ev.data.fd = sock_fd;

  if (epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &ev)) {
    printf("Error in setting epoll\n");  
    exit(1);
  }

  printf("Connection Established Successfully\n\n"); 
  xo_client.conn_fd = sock_fd;
 
  while (1)
  {
    timeout++;
    num_events = epoll_wait(efd, events, 10, 10000);
    // printf("Timeout = %d\n", timeout);
    for (i = 0 ; i < num_events; i++)
    {
      //if (sock_fd == events[i].data.fd)
     // printf("event on conn_fd = [%d]\n", events[i].data.fd);
      xo_client.conn_fd = events[i].data.fd;
      read_from_server(events[i].data.fd);
      timeout = 0; 

    }
    if (timeout > 5){
      printf("Connection has been lost.\n\n");
    if (xo_client.entity == GAME)
      display_game_options();
    else if (xo_client.entity == CHAT)
      display_chat_options();
    }
   
  }
return sock_fd;
}


/*----------------------------------------------------------
Function to handle Interrupt signals

------------------------------------------------------------*/
void xo_signal_handler(int signum)
{
  char buff[] = "CLOSE";
  XODL_CLI("Method called.\n");
  write_to_server(xo_client.conn_fd, buff, strlen(buff));
  close(xo_client.conn_fd);
  printf("Connection has been closed\n\n");
  exit (1);

}


/*----------------------------------------------------------
Function to start session with server

------------------------------------------------------------*/
int start_session_with_server()
{
  int bytes_written;
  XODL_CLI("Method called.\n");
  write_to_server(xo_client.conn_fd, xo_client.name, strlen(xo_client.name));

return SUCCESS;
}


/*---------------------------------------------------------
Function to write buffer to server

----------------------------------------------------------*/
int write_to_server(int conn_fd, char *buff, int size)
{
  int bytes_written;
  char temp_buff[] = "\\r\\n";
  XODL_CLI("Method called.\n");
  strcat(buff,temp_buff);
  while(1)
  {
    bytes_written = write(conn_fd, buff, strlen(buff));
    if (bytes_written < strlen(buff))
      continue;
    else
      break;
  }

 // XODL_CLI("Bytes written to server = [%d]\n", strlen(buff));
  XODL_CLI("Buff written to server = [%s]\n\n", buff);

return SUCCESS;

}

/*----------------------------------------------------------
Function to read buffer from server

----------------------------------------------------------*/
int read_from_server(int conn_fd)
{
  int bytes_read = 0, total_bytes_read = 0;
  char *temp;
  XODL_CLI("Method called.\n");

  while(1)
  {
    bytes_read = read(conn_fd, xo_client.read_buff + total_bytes_read, 1024);
    total_bytes_read += bytes_read;
    temp = strstr(xo_client.read_buff, "\\r\\n");
    if (temp == NULL) {
      continue;
    }
    else {
      *temp = 0;
      break;
    }
  }

 // XODL_CLI("Bytes read from server = [%d]\n", total_bytes_read);
  XODL_CLI("Buff read from server = [%s]\n", xo_client.read_buff);

  proc_server_resp();

return SUCCESS;
}

/*----------------------------------------------------------
Function to request list of online players from server

-----------------------------------------------------------*/
int get_online_players()
{
  char req[] = "ONLINE_PLAYERS";
  int count = 0;
  XODL_CLI("Method called.\n");
  write_to_server(xo_client.conn_fd, req, strlen(req));
  //read_from_server(xo_client.conn_fd);

 
return SUCCESS;
}

/*----------------------------------------------------------
Function to parse list of online players from read_buff

-----------------------------------------------------------*/

int parse_list_of_players()
{
  char *player_name;
  int count = 0;
  XODL_CLI("Method called.\n");
  if (strstr(xo_client.read_buff, "|") != NULL) {
    player_name = strtok(xo_client.read_buff+15, "|");
    printf("%d. %s\n", ++count, player_name);
    while(player_name != NULL)
    {
      player_name = strtok(NULL, "|");
      if (player_name != 0)
      printf("%d. %s\n", ++count, player_name);
    }

    printf("\nEnter the player no. to play with...");
    scanf("%d", &count);
    printf("\n");
    printf("\nWaiting for Remote Player...");
    printf("\n\n");
    create_player_connection(count);
  }
  else {
    printf("No one is online. Please try Later.\n\n");
    display_game_options(); 
  } 
return SUCCESS;
}

/*----------------------------------------------------------
Function to create connection with remote Player

-----------------------------------------------------------*/
int create_player_connection(int player)
{
  char req[100];
  XODL_CLI("Method called.\n");
  sprintf(req, "GAME_REQUEST:%d", player);
  write_to_server(xo_client.conn_fd, req, strlen(req));
  //read_from_server(xo_client.conn_fd);
  
return SUCCESS;
}

/*-----------------------------------------------------------
Function to init XO_Game

------------------------------------------------------------*/
int init_game_vars()
{
  char buff[] = "STATUS:ONLINE";
  XODL_CLI("Method called.\n");
  xo_client.status = ONLINE;
  xo_client.result = -1;
  xo_client.game_type == XO_3X3 ? init_grid_3() : init_grid_4() ;
  write_to_server(xo_client.conn_fd, buff, strlen(buff));
  
return SUCCESS;
}



/*----------------------------------------------------------
Function to process remote players game request
Note: This is a kind of interrupt to current process.

----------------------------------------------------------*/
int proc_game_request()
{
  char buff[] = "REQUEST_ACCEPTED";
  char ch;
  XODL_CLI("Method called.\n");
  printf("%s\n\n", xo_client.read_buff);
  printf("Do you want to accept(y/n)...");
  scanf("%c", &ch);
  scanf("%c", &ch);
  if (ch == 'y') {
    write_to_server(xo_client.conn_fd, buff, strlen(buff));
    if (strstr(xo_client.read_buff, "3X3") != NULL)
      xo_client.game_type = XO_3X3;
    else
      xo_client.game_type = XO_4X4;
    XODL_CLI("Game type = [%d]", xo_client.game_type);   
    xo_client.status = PLAYING;
    xo_client.state = READ;
    xo_client.player = 2;
    xo_client.game_type == XO_3X3 ? print_grid_3() : print_grid_4() ;
    handle_move();
  }
  //else
  //  read_from_server(xo_client.conn_fd);
     
return SUCCESS;
}

/*-------------------------------------------------------------
Function to accept Game type

--------------------------------------------------------------*/
int game_type()
{
  char buff[20];
  XODL_CLI("Method called.\n");
  printf("1. Play XO 3X3\n");
  printf("2. Play XO 4X4\n\n");
  printf("Enter your choice...");
  scanf("%d",&xo_client.game_type);
  sprintf(buff, "GAME_TYPE:%d", xo_client.game_type);
  write_to_server(xo_client.conn_fd, buff, strlen(buff));
  printf("\n\n");

return SUCCESS;
}

/*-------------------------------------------------------------
Function to display Game options

--------------------------------------------------------------*/
int display_game_options()
{
  int choice;
  XODL_CLI("Method called.\n");
  init_game_vars();
  game_type();
  printf("1. Play with any random player\n");
  printf("2. Get List of all players\n");
  printf("3. Exit\n\n");
  printf("Enter your choice...");
  scanf("%d",&choice);
  switch(choice)
  {
    case 1:
     // printf("Playing with random players");
     // break;
    case 2:
      printf("\nGetting list of online players...\n\n");
      get_online_players();
      break; 
    case 3:
      xo_signal_handler(9);
      break;
    default: 
      printf("Wrong choice.\n\n");
      display_game_options();
  }

return SUCCESS;
}

/*----------------------------------------------------------
Function to process Player's Move

-----------------------------------------------------------*/
int proc_player_move()
{
  int value;
  XODL_CLI("Method called.\n");
  xo_client.move = atoi(xo_client.read_buff + 5);
  xo_client.game_type == XO_3X3 ? proc_grid_3() : proc_grid_4() ;
  xo_client.state = WRITE;
  if (xo_client.result == WON || xo_client.result == LOST || xo_client.result == DRAW)
    reset_game();
  handle_move(); 

return SUCCESS;
}

/*----------------------------------------------------------
Function to handle player's move

-----------------------------------------------------------*/
int handle_move()
{
  int value;
  char buff[20];

  XODL_CLI("Method called.\n");
  //print_grid_3();
  if (xo_client.state == READ){
    printf("Waiting for remote player....");
    printf("\n\n");
    return;
  }
  else {
    if (xo_client.state == WRITE){
      printf("Now your Turn...");
      scanf("%d", &xo_client.move);
      sprintf(buff, "MOVE:%d", xo_client.move);
      write_to_server(xo_client.conn_fd, buff, strlen(buff));
      xo_client.game_type == XO_3X3 ? proc_grid_3() : proc_grid_4() ;
      //printf("Waiting for remote player...");
      //printf("\n\n");
      xo_client.state = READ;
      if (xo_client.result == WON || xo_client.result == LOST || xo_client.result == DRAW)
        reset_game();
      printf("Waiting for remote player...");
      printf("\n\n");

    }
  }
return SUCCESS;
}


/*-----------------------------------------------------------
Function to  process server response

------------------------------------------------------------*/
int proc_server_resp()
{
  XODL_CLI("Method called.\n");
  if (strncmp(xo_client.read_buff, "WELCOME TO XO_SERVER", 20) == 0){
    printf("%s\n\n", xo_client.read_buff); 
    start_session_with_server();
    if (xo_client.entity == GAME)
      display_game_options();
    if (xo_client.entity == CHAT)
      display_chat_options();
  } 
  else if (strncmp(xo_client.read_buff, "ONLINE_PLAYERS", 14) == 0){     
    if (xo_client.status == ONLINE){ 
      if (xo_client.entity == GAME)
        parse_list_of_players();
      else if (xo_client.entity == CHAT)
        parse_list_of_users();
    }
  }
  else if (strncmp(xo_client.read_buff, "GAME_REQUEST", 12) == 0) 
    proc_game_request();
  else if (strncmp(xo_client.read_buff, "CHAT_REQUEST", 12) == 0) 
    proc_chat_request();
  else if (strncmp(xo_client.read_buff, "REQUEST_ACCEPTED", 16) == 0){
         if (xo_client.entity == GAME) {  
           xo_client.state = WRITE;
           xo_client.player = 1;
           xo_client.result = -1;
           xo_client.game_type == XO_3X3 ? print_grid_3() : print_grid_4() ;
           handle_move();
         }
         else if (xo_client.entity == CHAT)
            handle_chat();
  }
  else if (strncmp(xo_client.read_buff, "MOVE:", 5) == 0)
    proc_player_move();
  else if (strncmp(xo_client.read_buff, "CHAT:", 5) == 0)
    proc_user_chat();
  else if (strncmp(xo_client.read_buff, "REQUEST_REJECTED", 16) == 0){
    printf("Sorry Your Request has been Rejected\n\n");
    display_game_options();
  }

return SUCCESS;
}

/*---------------------------------------------------
Function to restart game 

----------------------------------------------------*/
int reset_game()
{
  char ch;
  XODL_CLI("Method called.\n");

  printf("Do you want to play again(y/n)...");
  scanf("%c", &ch);
  scanf("%c", &ch);
  if (ch == 'y') {
  xo_client.result = -1; 
  xo_client.game_type == XO_3X3 ? init_grid_3() : init_grid_4() ;
  xo_client.game_type == XO_3X3 ? print_grid_3() : print_grid_4() ;
  handle_move();
  }
  else{
    display_game_options();
  }

return SUCCESS;
}

/*----------------------------------------------------
Function to initialize xo_client  structure

-----------------------------------------------------*/
int xo_client_init()
{
  XODL_CLI("Method called.\n");
  printf("\n\n\t*****WELCOME TO XO*****\n\n");
  printf("Please Enter your Name : ");
  gets(xo_client.name);
  xo_client.status = ONLINE;

return SUCCESS;
}

/*----------------------------------------------------------
Function to parse arguments

-----------------------------------------------------------*/
int parse_arguments(int argc, char *argv[])
{
  char option;
  XODL_CLI("Method called.\n");

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


/*---------------------------------------------------
Main Function for XO_Client

-----------------------------------------------------*/
int main(int argc, char *argv[])
{
  int sock_fd, choice, online_players;
  char player_name[100];
  XODL_CLI("Method called.");
   
  parse_arguments(argc, argv);
  xo_client_init();
  printf("\n");
  printf("1. PLay Game Online\n");
  printf("2. Chat \n\n");
  printf("Enter your choice...");
  scanf("%d",&choice);
  signal(SIGINT, xo_signal_handler);
  switch(choice)
  {
    case 1:
      xo_client.entity = GAME;
      printf("\n\nConnecting to server...");
      xo_client.conn_fd = create_tcp_connection();
      //start_session_with_server();
      break;
    case 2:
      xo_client.entity = CHAT;
      printf("\n\nConnecting to server...");
      xo_client.conn_fd = create_tcp_connection();

  } 


return SUCCESS;
}




