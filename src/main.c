#include "lemipc.h"
#include "libft.h"
#include "shared_resources.h"
#include "log.h"

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

int	main(int argc, char **argv)
{
	t_shared_resources	shared_rcs = {};
	key_t				key = generateSysVKey(1);
	int					team_id;


	team_id = parseTeamId(argc, argv);
	if (team_id == -1)
		return 1;

	if (getSharedResources(&shared_rcs, key) == -1)
		return 1;


	if (cleanSharedResources(&shared_rcs, CLEAN_ALL) == -1)
	{
		
	}
	return 0;
}
