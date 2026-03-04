#include "ANSI-color-codes.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef DEBUG
  void	log_verb(const char *msg)
  {
	  fprintf(stdout, BMAG "[ VERBOSE ]" CRESET " %s\n", msg);
  }

  void	log_verb_code(const char *msg, const int n)
  {
	  fprintf(stdout, BMAG "[ VERBOSE ]" CRESET " %s %i\n", msg, n);
  }
#else
  void	log_verb(const char *msg)
  {
	  (void)msg;
  }

  void	log_verb_code(const char *msg, const int n)
  {
	  (void)msg;
	  (void)n;
  }
#endif

void	log_err(const char *msg)
{
	fprintf(stderr, BRED "[  ERROR  ]" CRESET " %s\n", msg);
}

void	log_err_code(const char *msg, const int n)
{
	fprintf(stderr, BRED "[  ERROR  ]" CRESET " %s %i\n", msg, n);
}

void	log_syserr(const char *msg)
{
	char	*str_err = strerror(errno);
	fprintf(stderr, BRED "[  ERROR  ]" CRESET " %s %s\n", str_err, msg);
}

void	log_war(const char *msg)
{
	fprintf(stderr, BYEL "[ WARNING ]" CRESET " %s\n", msg);
}

void	log_info(const char *msg)
{
	fprintf(stdout, BBLU "[  INFO   ]" CRESET " %s\n", msg);
}
