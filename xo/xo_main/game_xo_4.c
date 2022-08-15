#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "xo_client.h"
#include "xo_debug.h"

#define RESET   "\033[0m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDMAGENTA "\033[1m\033[35m" 
#define BOLDRED     "\033[1m\033[31m" 


char board_4[4][4];
static int total_chance;

/*-------------------------------
Function to initiallize board

--------------------------------*/

int init_grid_4()
{
  int i,j;
  XODL_CLI("Method called. \n");
  total_chance = 0;
  for(i = 0; i < 4; i++) {
    for(j = 0; j<4; j++) {
      board_4[i][j] = '\0';
    }
  }

return SUCCESS;
}

/*-------------------------------------------------------------
Function to print the board on screen

--------------------------------------------------------------*/

int print_grid_4() 
{
  int i,j;
  system("clear");
  XODL_CLI("Method called. \n");
  printf("\n\n");
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (board_4[i][j] == '\0') 
        printf(" %2d ", 4*(i)+(j+1));
      else 
        printf(BOLDGREEN  " %c  " RESET, board_4[i][j]);
      if (j!=3)  
        printf(BOLDRED "|" RESET); 
    }
    if (i != 3) 
      printf(BOLDRED "\n-------------------\n" RESET);
  }
  printf("\n\n\n");

return SUCCESS;
}

/*------------------------------------------------------------------------
Function to check winnner for the game

-------------------------------------------------------------------------*/

int check_winner_4() {
  int i,j;
  char current;
  char winner = '\0';
  XODL_CLI("Method called. \n");

  //Iter over rows to check for winner
  for(i = 0; i<4; i++) {
    current = board_4[i][0];
    for(j = 0; j < 4; j++) {
      if(board_4[i][j] != current) {
    current = '\0';
      }
    }
    if(current != '\0') {
      winner = current;
    }
  }

  //Iter over columns
  for(i = 0; i<4; i++) {
    current = board_4[0][i];
    for(j = 0; j < 4; j++) {
      if(board_4[j][i] != current) {
    current = '\0';
      }
    }
    if(current != '\0') {
      winner = current;
    }
  }
 //Iter over diagonals
  current = board_4[0][0];
  for(i = 0; i < 4; i++) {
    if(board_4[i][i] != current) {
      current = '\0';
    }
  }
  if(current != '\0') {
    winner = current;
  }

  current = board_4[0][4];
  for(i = 0; i <4; i++) {
    if(board_4[i][4-i] != current) {
      current = '\0';
    }
  }
  if(current != '\0') {
    winner = current;
  }

  if (winner != '\0' && xo_client.state == WRITE){
    printf("\n\nCongrats! you WON.\n\n");
    xo_client.result = WON;
  } 
  else if (winner != '\0' && xo_client.state == READ){
    printf("\n\nSorry! you LOOSE.\n\n");
    xo_client.result = LOST;
  } 
  else if (total_chance >= 16){
    printf("Game Draw! \n\n");
    xo_client.result = DRAW;
  }
    
return SUCCESS;
}

/*---------------------------------------
Function to mark 'X' or 'O' in the board

-----------------------------------------*/

int proc_grid_4()
{
  int nrows = 0, ncols = 0, value = 0;
  XODL_CLI("Method called. \n");
  value = xo_client.move;
  value--;
  ncols = value%4;
  value = value - ncols;
  nrows = value/4;
  if (xo_client.state == WRITE)
    board_4[nrows][ncols] = 'X';
  else if (xo_client.state == READ)
    board_4[nrows][ncols] = 'O';
  total_chance++;
  print_grid_4();
  check_winner_4();
  
return 0;
}
                         
