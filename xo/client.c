#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main( int argc, char * argv[])
{
  struct sockaddr_in client;
  int fd, count = 0 , data_sent;
  char *buff;

  memset(&client, 0 , sizeof(client));
  buff = (char*)malloc(1024);

  fd = socket(AF_INET, SOCK_STREAM, 0);
  
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = inet_addr("10.10.70.57");
  client.sin_port = htons(1234);
  
  connect(fd, (struct sockaddr *)&client, sizeof(client));

  while(1)
  {   
    printf("Client: ");
    gets(buff);
    //printf(" %s\n", buff);
    data_sent = write(fd, buff, strlen(buff));
    read(fd, buff, 1024);
    printf("Server: %s\n", buff, data_sent);
  }

  return 0;
}
