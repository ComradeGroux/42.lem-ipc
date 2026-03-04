#include "lemipc.h"
#include "shared_resources.h"
#include "log.h"

#include <stddef.h>
#include <stdlib.h>
#include <signal.h>

bool	gIsSigReceived = false;
bool	gIsSemLocked = false;

static void	signalHandler(int signo, siginfo_t *sinfo, void *context)
{
	(void)sinfo;
	(void)context;
	if (signo == SIGINT || signo == SIGTERM)
		gIsSigReceived = true;
}

int	initSignalHandler(void)
{
	struct sigaction sa = {0};

	sa.sa_sigaction = &signalHandler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGTERM);
	sa.sa_flags = SA_SIGINFO;

	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		log_syserr("(sigaction - SIGINT)");
		return -1;
	}
	if (sigaction(SIGTERM, &sa, NULL) == -1)
	{
		log_syserr("(sigaction - SIGTERM)");
		return -1;
	}
	return 0;
}
