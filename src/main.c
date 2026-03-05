#include "game_mode.h"
#include "libft.h"
#include "log.h"
#include "signal_handling.h"
#include "shared_resources.h"

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

extern volatile __sig_atomic_t	gIsSigReceived;
extern volatile __sig_atomic_t	gIsSemLocked;

static int	parseTeamId(int argc, char **argv)
{
	int	team_id;

	if (argc != 2)
	{
		log_err("Wrong number of arguments");
		log_info("Usage: ./lemipc TEAM-ID");
		return -1;
	}

	team_id = ft_atoi(argv[1]);
	if (team_id < 0 || team_id >= NB_MAX_TEAMS)
	{
		log_err_code("Team number must be a number between 0 and ", NB_MAX_TEAMS);
		log_info("Usage: ./lemipc TEAM-ID");
		return -1;
	}

	return team_id;

}

static int	quit(t_shared_resources *shared_rcs)
{
	if (gIsSemLocked == true)
		semUnlock(shared_rcs->sem_id);
	cleanSharedResources(shared_rcs, CLEAN_ALL);
	_exit(EXIT_FAILURE);
}

int	main(int argc, char **argv)
{
	t_shared_resources	shared_rcs = {};
	int					team_id;
	t_map_info			*map;
	t_player			player;

	team_id = parseTeamId(argc, argv);
	if (team_id == -1)
		return EXIT_FAILURE;
	else
		log_verb_code("team_id is", team_id);
	if (initSignalHandler() == -1)
		return EXIT_FAILURE;
	if (getSharedResources(&shared_rcs, generateSysVKey(1)) == IPC_RESULT_ERROR)
		return EXIT_FAILURE;
	if (initSharedResources(&shared_rcs, &map) == IPC_RESULT_ERROR)
		quit(&shared_rcs);

	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
	{
		log_syserr("(clock_gettime - CLOCK_MONOTONIC)");
		quit(&shared_rcs);
	}
	srandom(ts.tv_sec);

	player.team = (unsigned int)team_id;
	if (player.team == 0)
	{
		if (graphicMode(&shared_rcs, map) == -1)
		{
			cleanSharedResources(&shared_rcs, CLEAN_ALL);
			return EXIT_FAILURE;
		}
	}
	else
	{
		if (playerMode(&shared_rcs, map, &player) == -1)
		{
			cleanSharedResources(&shared_rcs, CLEAN_ALL);
			return EXIT_FAILURE;
		}
	}

	if (gIsSigReceived == true)
		log_info("Signal received, quitting");
	if (cleanSharedResources(&shared_rcs, CLEAN_ALL) <= IPC_RESULT_ERROR)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
