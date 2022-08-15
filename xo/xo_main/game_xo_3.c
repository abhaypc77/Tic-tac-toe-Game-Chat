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

char board_3[3][3] = 
    {
        {'1','2','3'},
        {'4','5','6'},
        {'7','8','9'}
    };

static int total_chance;

/*-------------------------------
Function to initiallize board

--------------------------------*/

int init_grid_3()
{
  int i, j, k=49;
  total_chance = 0;
  XODL_CLI("Method called. \n");
  for (i=0; i<3; i++)
    for (j=0; j<3; j++)
      board_3[i][j] = k++;

return SUCCESS;
}

/*---------------------------------------
Function to mark 'X' or 'O' in the board

-----------------------------------------*/

int proc_grid_3()
{
  int row, column;

  XODL_CLI("Method called. \n");
  row = (xo_client.move - 1) / 3;
  column = (xo_client.move - 1) % 3;
  if (xo_client.state == WRITE)
    board_3[row][column] = 'X';
  else if (xo_client.state == READ)
    board_3[row][column] = 'O';
  total_chance++;
  print_grid_3();
  check_winner_3();

return SUCCESS;
}

/*-------------------------------------------------------------
Function to print the board on screen

--------------------------------------------------------------*/
int print_grid_3()
{  
  system("clear");
  XODL_CLI("Method called. \n");
  printf("\n\n");
  printf(BOLDGREEN " %c | %c | %c\n" RESET, board_3[0][0], board_3[0][1], board_3[0][2]);
  printf(BOLDGREEN "-----------\n" RESET);
  printf(BOLDGREEN " %c | %c | %c\n" RESET, board_3[1][0], board_3[1][1], board_3[1][2]);
  printf(BOLDGREEN "-----------\n" RESET);
  printf(BOLDGREEN " %c | %c | %c\n" RESET, board_3[2][0], board_3[2][1], board_3[2][2]);
  printf("\n\n\n");
return SUCCESS;
}

/*------------------------------------------------------------------------
Function to check winnner for the game

-------------------------------------------------------------------------*/

check_winner_3()
{
  int line = 0, winner = 0;
  XODL_CLI("Method called. \n");
  if ((board_3[0][0] == board_3[1][1] && board_3[0][0] == board_3[2][2]) ||
      (board_3[0][2] == board_3[1][1] && board_3[0][2] == board_3[2][0]))
    winner = 1;
  else
    for(line = 0; line < 3; line ++)
      if ((board_3[line][0] == board_3[line][1] && board_3[line][0] == board_3[line][2])||
          (board_3[0][line] == board_3[1][line] && board_3[0][line] == board_3[2][line]))
        winner = 1;
  if (winner == 1 && xo_client.state == WRITE){
    printf("\n\nCongrats! you WON.\n\n");
    xo_client.result = WON; 
  }
  else if (winner == 1 && xo_client.state == READ){
    printf("\n\nSorry! you LOOSE. \n\n");
    xo_client.result = LOST;
  }
  else if (total_chance >= 9){
    printf("Game Draw! \n\n");
    xo_client.result = DRAW;
  }
    
return SUCCESS;
}


