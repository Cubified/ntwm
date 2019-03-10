/*
 * util.h: various utility functions
 */

#ifndef __UTIL_H
#define __UTIL_H

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

static void spawn(char *cmd);
static int hash_str(char *str);

/*
 * Informs ntwm to quit by
 * flipping the running
 * boolean
 */
#define quit() (running = 0)

/*
 * Forks a child process,
 * used with the "spawn"
 * command in config.h
 */
void spawn(char *cmd){
  pid_t process = fork();

  if(process < 0){
    error("Could not fork process.");
  } else if(process == 0){
    char *argv[2];
    int retval;

    argv[0] = cmd;
    argv[1] = NULL;
    retval = execvp(cmd, argv);
    if(retval == -1){
      error("Failed to start process.");
    }
  }
}

/*
 * Rounds a float to
 * the nearest int
 */
#define round_float(n) ((int)(n < 0 ? n - 0.5 : n + 0.5))

/*
 * Finds the closest
 * even number to
 * an integer n
 */
#define closest_even(n) (n % 2 == 0 ? n : n - 1)

/*
 * Finds the closest
 * odd number to
 * an integer n
 */
#define closest_odd(n) (n % 2 ? n : n - 1)

/*
 * "Hashes" a string to
 * avoid a strcmp()
 * if/else ladder
 */
int hash_str(char *str){
  int out = 0;
  char *c;

  for(c=str;*c;c++){
    out += *c - '0';
  }

  return out;
}


#endif
