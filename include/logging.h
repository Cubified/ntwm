/*
 * logging.h: log functions
 */

#ifndef __LOGGING_H
#define __LOGGING_H

#ifndef LOGGING_NO_STDIO
#include <stdio.h>
#include <stdarg.h>
#endif

#define STDOUT 1
#define STDERR 2

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static int  len(char *str);
static void print(char *str);
static void info(char *str, ...);
static void error(char *str, ...);
static void debug(char *str, ...);

int len(char *str){
  int o;
  char *c;
  for(c=str,o=0;*c;c++,o++){}
  return o;
}

void print_len(char *str, int len){
  write(STDOUT, str, len);
}

void print(char *str){
  print_len(str, len(str));
}

void print_err_len(char *str, int len){
  write(STDERR, str, len);
}

void print_err(char *str){
  print_err_len(str, len(str));
}

/***/

void info(char *str, ...){
#ifdef LOGGING_NO_STDIO
  print_len(ANSI_COLOR_CYAN "=> ", 8);
  print(str);
  print_len(ANSI_COLOR_RESET "\n", 5);
#else
  va_list args;

  va_start(args,str);
  printf(ANSI_COLOR_CYAN "=> ");
  vfprintf(stdout,str,args);
  printf(ANSI_COLOR_RESET "\n");
  va_end(args);
#endif
}

void error(char *str, ...){
#ifdef LOGGING_NO_STDIO
  print_err_len(ANSI_COLOR_RED "==> Error: ", 16);
  print_err(str);
  print_err_len(ANSI_COLOR_RESET "\n", 5);
#else
  va_list args;

  va_start(args,str);
  printf(ANSI_COLOR_RED "==> Error: ");
  vfprintf(stdout,str,args);
  printf(ANSI_COLOR_RESET "\n");
  va_end(args);
#endif
}

void debug(char *str, ...){
#ifdef LOGGING_NO_STDIO
  print_len(ANSI_COLOR_GREEN "=> Debug: ", 15);
  print(str);
  print_len(ANSI_COLOR_RESET "\n", 5);
#else
  va_list args;

  va_start(args,str);
  printf(ANSI_COLOR_GREEN "=> Debug: ");
  vfprintf(stdout,str,args);
  printf(ANSI_COLOR_RESET "\n");
  va_end(args);
#endif
}

#endif
