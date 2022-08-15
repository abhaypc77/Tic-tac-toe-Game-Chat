#include <stdio.h>          
#include <sys/socket.h>    
#include <arpa/inet.h>    
#include <stdlib.h>     
#include <string.h> 
#include <unistd.h>        
#include <fcntl.h>        
#include <errno.h>
#include <sys/epoll.h> 

int main(int argc, char *argv[])
{
  struct sockaddr_in server;
  struct epoll_event ev;
  struct epoll_event events[10];
  int i = 0, j = 0, fd, efd, num_events, con_fd, data_read;
  char buff[1024] = "aaaa";
  
  memset(&server, 0 ,sizeof(server) );
  fd = socket(AF_INET, SOCK_STREAM, 0);
  efd = epoll_create(10);
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(1234);

  bind(fd, (struct sockaddr *)&server, sizeof(server) );
  fcntl(fd, F_SETFL, O_NONBLOCK);
  listen(fd, 1);

  ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
  ev.data.fd = fd;

  epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev);
  
  while (1)
  { 
    num_events = epoll_wait(efd, events, 10, 10000);

    for (i = 0 ;i < num_events; i++)
    {
      if (fd == events[i].data.fd) {
        con_fd = accept(fd, (struct sockaddr *)NULL, NULL);
        printf("Connection established con_fd == %d\n\n", con_fd);
        ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
        ev.data.fd = con_fd;
        epoll_ctl(efd, EPOLL_CTL_ADD, con_fd, &ev);
      } else {
          data_read = read(events[i].data.fd, buff, 1024);
          printf ("Client-%d: %s\n", events[i].data.fd, buff); 
          printf ("Server: "); 
          gets(buff);
          write(events[i].data.fd, buff, 1024);
      }

    }
  }  
    
  return 0;
}
