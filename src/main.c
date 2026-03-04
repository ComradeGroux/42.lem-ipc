#include "lemipc.h"
#include "log.h"
#include "libft.h"
#include "game_utils.h"
#include "shared_resources.h"

extern bool	gIsSigReceived;
extern bool	gIsSemLocked;

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
	if (team_id < 0 || team_id > NB_MAX_TEAMS)
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
	{
		// sem_unlock(shared_rcs->sem_id);
	}
	cleanSharedResources(shared_rcs, CLEAN_ALL);
	_exit(EXIT_FAILURE);
}

int	main(int argc, char **argv)
{
	t_shared_resources	shared_rcs = {};
	int					team_id;
	t_map_info			map;
	t_player			player;

	team_id = parseTeamId(argc, argv);
	if (team_id == -1)
		return EXIT_FAILURE;
	if (initSignalHandler() == -1)
		return EXIT_FAILURE;
	if (getSharedResources(&shared_rcs, generateSysVKey(1)) == IPC_RESULT_ERROR)
		return EXIT_FAILURE;
	if (initSharedResources(&shared_rcs, &map) == IPC_RESULT_ERROR)
		quit(&shared_rcs);

	player.team = team_id;
	if (player.team == 0)
	{
		// GRAPHICAL LOOP
	}
	else
	{
		//  GAME LOOP
	}


	if (gIsSigReceived == true)
		log_info("Signal received, quitting");
	if (cleanSharedResources(&shared_rcs, CLEAN_ALL) < 0)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
