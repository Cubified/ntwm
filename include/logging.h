/*
 * logging.h: log functions
 */

#ifndef __LOGGING_H
#define __LOGGING_H

#ifdef LOGGING
#include <stdio.h>
#include <stdarg.h>
#endif

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static void info(const char *str, ...);
static void error(const char *str, ...);
static void debug(const char *str, ...);

void info(const char *str, ...){
#ifdef LOGGING
  va_list args;

  va_start(args,str);
  printf(ANSI_COLOR_CYAN "=> ");
  vfprintf(stdout,str,args);
  printf(ANSI_COLOR_RESET "\n");
  va_end(args);
#endif
}

void error(const char *str, ...){
#ifdef LOGGING
  va_list args;

  va_start(args,str);
  printf(ANSI_COLOR_RED "==> Error: ");
  vfprintf(stdout,str,args);
  printf(ANSI_COLOR_RESET "\n");
  va_end(args);
#endif
}

void debug(const char *str, ...){
#ifdef LOGGING
  va_list args;

  va_start(args,str);
  printf(ANSI_COLOR_GREEN "=> Debug: ");
  vfprintf(stdout,str,args);
  printf(ANSI_COLOR_RESET "\n");
  va_end(args);
#endif
}

#endif
