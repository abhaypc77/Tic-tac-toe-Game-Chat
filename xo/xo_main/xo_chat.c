#include <stdio.h>
#include <string.h>
#include "xo_debug.h"
#include "xo_client.h"



/*----------------------------------------------------------
Function to create user connection 

-----------------------------------------------------------*/

int create_user_connection(int user)
{
  char req[100];
  XODL_CLI("Method called.\n");
  sprintf(req, "CHAT_REQUEST:%d", user);
  write_to_server(xo_client.conn_fd, req, strlen(req));

return 0;
}

/*----------------------------------------------------------
Function to handle chat

-----------------------------------------------------------*/
int handle_chat()
{
  int value;
  char buff[1024], chat[1024];

  XODL_CLI("Method called.\n");
  gets(chat);
  sprintf(buff, "CHAT:%s", chat);
  write_to_server(xo_client.conn_fd, buff, strlen(buff));
  //read_from_server();
  
return SUCCESS;
}


/*----------------------------------------------------------
Function to handle chat

-----------------------------------------------------------*/
int proc_user_chat()
{
  XODL_CLI("Method called.\n");
  printf("User : %s\n", xo_client.read_buff + 5);
  printf("You  : ");
  handle_chat();

return SUCCESS;
}

/*----------------------------------------------------------
Function to process remote users chat request
Note: This is a kind of interrupt to current process.

----------------------------------------------------------*/
int proc_chat_request()
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
    xo_client.status = BUSY;
  }

return SUCCESS;
}


/*----------------------------------------------------------
Function to parse list of online players from read_buff

-----------------------------------------------------------*/

int parse_list_of_users()
{
  char *user_name;
  int count = 0;
  XODL_CLI("Method called.\n");
  if (strstr(xo_client.read_buff, "|") != NULL) {
    user_name = strtok(xo_client.read_buff+15, "|");
    printf("%d. %s\n", ++count, user_name);
    while(user_name != NULL)
    {
      user_name = strtok(NULL, "|");
      if (user_name != 0)
      printf("%d. %s\n", ++count, user_name);
    }

    printf("\nEnter the user no. you want to chat with...");
    scanf("%d", &count);
    printf("\n");
    printf("\nWaiting for Remote User...");
    printf("\n\n");
    create_user_connection(count);
  }
  else {
    printf("No one is online. Please try Later.\n\n");
    display_chat_options();
  }
return SUCCESS;
}


/*----------------------------------------------------------
Function to request list of online users from server

-----------------------------------------------------------*/
int get_online_users()
{
  char req[] = "ONLINE_USERS";
  int count = 0;
  XODL_CLI("Method called.\n");
  write_to_server(xo_client.conn_fd, req, strlen(req));
  //read_from_server(xo_client.conn_fd);


return SUCCESS;
}


/*-------------------------------------------------------------
Function to display chat options

--------------------------------------------------------------*/
int display_chat_options()
{
  int choice;
  XODL_CLI("Method called.\n");
  printf("1. Chat with any random user\n");
  printf("2. Get List of all online users\n");
  printf("3. Exit\n\n");
  printf("Enter your choice...");
  scanf("%d",&choice);
  switch(choice)
  {
    case 1:
     // printf("Playing with random players");
     // break;
    case 2:
      printf("\nGetting list of online users...\n\n");
      get_online_users();
      break;
    case 3:
      xo_signal_handler(9);
      break;
    default:
      printf("Wrong choice.\n\n");
      display_chat_options();
  }

return SUCCESS;
}





