#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <openssl/bio.h>
#include "xo_server.h"


#define MAX_DEBUG_SIZE 1024
#define SERVER_DEBUG_FILE "server_debug.log"
#define CLIENT_DEBUG_FILE "client_debug.log"


static char debug_header[] = "FILE_NAME|LINE|FUNCTION|\n";
static int debug_fd = -1;
extern int debug_flag;

void open_debug_file(char entity)
{
  if (debug_fd < 0) {
    if (entity == 'S')
    debug_fd = open(SERVER_DEBUG_FILE, O_CREAT|O_WRONLY|O_APPEND, 00666);
    else
    debug_fd = open(CLIENT_DEBUG_FILE, O_CREAT|O_WRONLY|O_APPEND, 00666);
    if (debug_fd <= 0)
    {
      fprintf(stderr, "Error: Error in opening file '%s'\n", (entity == 'S') ? SERVER_DEBUG_FILE : CLIENT_DEBUG_FILE);
      //exit (-1);
    }
    write(debug_fd, debug_header, strlen(debug_header));
  }


}

/*------------------------------------------------------------------
Function to handle debug logs

-------------------------------------------------------------------*/

void xo_debug_log(char entity, char *file, int line, char *fun_name, char *format, ...)
{
  va_list ap; 
  int bytes_written = 0, va_bytes_written = 0;
  char buff[MAX_DEBUG_SIZE];
  
  if (debug_flag == 0)
    return;

  bytes_written = sprintf(buff, "%s|%d|%s|", file, line, fun_name);
  va_start(ap, format);
  va_bytes_written = vsnprintf(buff + bytes_written, MAX_DEBUG_SIZE - bytes_written, format, ap); 
  va_end(ap);

  open_debug_file(entity);
  write(debug_fd, buff, bytes_written + va_bytes_written);

}



