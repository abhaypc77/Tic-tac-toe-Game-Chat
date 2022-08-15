#include <stdio.h>
#include <string.h>
//#include <inet/sock.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define RESET   "\033[0m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDMAGENTA "\033[1m\033[35m" 
#define BOLDRED     "\033[1m\033[31m" 

struct sockaddr_in client;
int sock_fd;

int create_tcp_connection()
{
  memset(&client, 0 , sizeof(client));
  //buff = (char*)malloc(1024);

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  client.sin_family = AF_INET;
  client.sin_addr.s_addr = inet_addr("127.0.0.1");
  client.sin_port = htons(1234);

  connect(sock_fd, (struct sockaddr *)&client, sizeof(client));

return 0;
}

int write_to_server(int value)
{
  int bytes_written;
  char buff[1024];
  sprintf(buff, "%d", value);
  bytes_written = write(sock_fd, buff, strlen(buff));

return 0;
}

int read_from_server()
{
  int bytes_read;
  char buff[1024];
  int value ;

 // printf("\n\nWaiting for Server..."); 
  read(sock_fd, buff, 1024);
  value = atoi(buff);
  printf("%d\n", value);
return value;
}

int print_grid_3(char board[3][3])
{
 system("clear");
 printf("\n\n");
        printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
printf("-----------\n");
        printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
printf("-----------\n");
        printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);
return 0;
}

int play_xo3(void)
{
    int i = 0;
    int player = 0;
    int value = 0;
    int row = 0; 
    int column = 0;
    int line = 0;
    int winner = 0;
    char board[3][3] = 
    {
        {'1','2','3'},
        {'4','5','6'},
        {'7','8','9'}
    };
   


 
    for( i = 0; i<9 && winner==0; i++)
    
    {
      print_grid_3(board);   
        
        player = i%2 + 1;
        
        do
        {
//printf("\nPlayer %d, please enter the number of the square "
//                   "where you want to place your %c: ", player,(player==1)?'X':'O');
      if (player == 1) {    
        printf("\n\nYour Turn..."); 
       scanf("%d", &value);
       write_to_server(value);
      }
      else {
        value = read_from_server();
         
      }
            
            row = --value/3;
            column = value%3;
        }
        while(value<0 || value>9 || board[row][column]>'9');
        
        board[row][column] = (player == 1) ? 'X' : 'O';
        
        if((board[0][0] == board[1][1] && board[0][0] == board[2][2]) ||
           (board[0][2] == board[1][1] && board[0][2] == board[2][0]))
            winner = player;
        else
            
            for(line = 0; line < 3; line ++)
                
                if((board[line][0] == board[line][1] && board[line][0] == board[line][2])||
                   (board[0][line] == board[1][line] && board[0][line] == board[2][line]))
                    winner = player;
        
    }
      print_grid_3(board);   
    
    if(winner == 0)
        printf("\nDraw\n");
    else
printf("\nplayer %d, YOU WON!\n", winner);

return 0;
}

void print_grid_4(char board[4][4]) {
  int i,j;
  printf("\n\n");
  for(i = 0; i < 4; i++) {
    for(j = 0; j < 4; j++) {
      if(board[i][j] == '\0') {
    printf(" %2d ", 4*(i)+(j+1));
      }
      else {
    printf(BOLDGREEN  " %c  " RESET, board[i][j]);
      }
      if(j!=3) { printf(BOLDRED "|" RESET); }
    }
    if(i != 3) {
      printf(BOLDRED "\n-------------------\n" RESET);
    }
  }
}



int didwin(char board[4][4]) {
  int i,j;
  char current;
  char winner = '\0';

  //Iter over rows to check for winner
  for(i = 0; i<4; i++) {
    current = board[i][0];
    for(j = 0; j < 4; j++) {
      if(board[i][j] != current) {
    current = '\0';
      }
    }
    if(current != '\0') {
      winner = current;
    }
  }

  //Iter over columns
  for(i = 0; i<4; i++) {
    current = board[0][i];
    for(j = 0; j < 4; j++) {
      if(board[j][i] != current) {
    current = '\0';
      }
    }
    if(current != '\0') {
      winner = current;
    }
  }
 //Iter over diagonals
  current = board[0][0];
  for(i = 0; i < 4; i++) {
    if(board[i][i] != current) {
      current = '\0';
    }
  }
  if(current != '\0') {
    winner = current;
  }

  current = board[0][4];
  for(i = 0; i <4; i++) {
    if(board[i][4-i] != current) {
      current = '\0';
    }
  }
  if(current != '\0') {
    winner = current;
  }
  return winner;
}


int play_xo4()
{
int i=0,j=0,player=0,value=0,nrows=0,ncols=0,winner=0;
  char again='q';
  char four_X_four[4][4];
   system("clear");
  while(winner == '\0') {
    for(i = 0; i < 4; i++) {
      for(j = 0; j<4; j++) {
    four_X_four[i][j] = '\0';
      }
    }
    print_grid_4(four_X_four);
    for( i = 0; i<16 && winner==0; i++)
      {
    player = i%2 + 1;
    if(player==1){
      printf("\n Player %d, please enter the number of the square "
         "where you want to place your %c: ",player,'X');
      scanf("%d", &value);
      write_to_server(value);
    }
    else if(player==2) {
      printf("\n Player %d, please enter the number of the square "
         "where you want to place your %c: ", player,'O');
      value = read_from_server();
    }
    value--;
    ncols = value%4;
    value = value - ncols;
    nrows = value/4;
    if(value<0 || value>16 || four_X_four[nrows][ncols]=='X' || four_X_four[nrows][ncols]=='O') {
      printf("Space is already taken, please try again");
      i--;
    }
    else {
 four_X_four[nrows][ncols] = (player == 1) ? 'X' : 'O';
    }
     system("clear");
    print_grid_4(four_X_four);
    winner = didwin(four_X_four);
      }
    if(winner != '\0') {
      printf("\nWinner was %c! Good job.\n",winner);
      printf("Do you want to play again? y/n: ");
      while(!(again == 'y' || again == 'n')) {
        scanf("%c",&again);
      }
      if(again == 'y') {
        winner = '\0';
        again = 'q';
      }
    }
    else {
      printf("\nNo winner this round. Try again.");
    }
  }
  return 0;
}
                         
int main(int argc, char *argv[])
{
  int game_type;

  create_tcp_connection();
  printf("\nEnter 3 or 4\n");
  scanf("%d",&game_type);
  write_to_server(game_type);
  if (game_type == 3)
    play_xo3();
  else if (game_type == 4)
    play_xo4();

return 0;
} 





