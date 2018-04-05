/*
 * logging.h: log functions
 */

#ifndef __LOGGING_H
#define __LOGGING_H

#include <stdio.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static void info(const char *str);
static void error(const char *str);
//static void debug(void *data,const char *str);

void info(const char *str){
  if(log_colors){
    printf(ANSI_COLOR_CYAN "=> %s" ANSI_COLOR_RESET "\n",str);
  } else {
    printf("=> %s\n",str);
  }
}
void error(const char *str){
  if(log_colors){
    printf(ANSI_COLOR_RED "==> Error: %s" ANSI_COLOR_RESET "\n",str);
  } else {
    printf("==> Error: %s\n",str);
  }
}
/*
void debug(void *data,const char *str){
  if(log_colors){
    printf(ANSI_COLOR_GREEN "=> Debug: " ANSI_COLOR_RESET "%s" ANSI_COLOR_GREEN "=" ANSI_COLOR_RESET "%p\n",str,data);
  } else {
    printf("=> Debug: %s=%p\n",str,data);
  }
}*/
#endif
