#include <stdio.h>
//#include "tic.h"
void print_grid(char board[4][4]);
int didwin(char board[4][4]);

int main(void){
  int i=0,j=0,player=0,lead=0,nrows=0,ncols=0,winner=0;
  char again='q';
  char four_X_four[4][4];
   system("clear");
  while(winner == '\0') {
    for(i = 0; i < 4; i++) {
      for(j = 0; j<4; j++) {
    four_X_four[i][j] = '\0';
      }
    }
    print_grid(four_X_four);
    for( i = 0; i<16 && winner==0; i++)
      {
    player = i%2 + 1;
    if(player==1){
      printf("\n Player %d, please enter the number of the square "
         "where you want to place your %c: ",player,'X');
    }
    else if(player==2) {
      printf("\n Player %d, please enter the number of the square "
         "where you want to place your %c: ", player,'O');
    }
    scanf("%d", &lead);
    lead--;
    ncols = lead%4;
    lead = lead - ncols;
    nrows = lead/4; 
    if(lead<0 || lead>16 || four_X_four[nrows][ncols]=='X' || four_X_four[nrows][ncols]=='O') {
      printf("Space is already taken, please try again");  
      i--;
    }
    else {
      four_X_four[nrows][ncols] = (player == 1) ? 'X' : 'O';
    }
     system("clear");
    print_grid(four_X_four);
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
void print_grid(char board[4][4]) {
  int i,j;
  printf("\n\n");
  for(i = 0; i < 4; i++) {
    for(j = 0; j < 4; j++) {
      if(board[i][j] == '\0') {
    printf(" %2d ", 4*(i)+(j+1));
      }
      else {
    printf(" %c  ", board[i][j]);
      }
      if(j!=3) { printf("|"); }
    }
    if(i != 3) {
      printf("\n-------------------\n");
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

