#include "game_utils.h"
#include "shared_resources.h"
#include "ANSI-color-codes.h"
#include "libft.h"
#include "log.h"
#include "game_private.h"

extern volatile __sig_atomic_t	gIsSigReceived;
extern volatile __sig_atomic_t	gIsSemLocked;

static int	chooseColor(int team)
{
	switch (team)
	{
		case 0:
			ft_printf("   |");
			return -1;
		case 1:
			ft_printf(BBLUBRIGHT);
			return 1;
		case 2:
			ft_printf(BMAGBRIGHT);
			return 1;
		case 3:
			ft_printf(BYEL);
			return 1;
		case 4:
			ft_printf(BCYNBRIGHT);
			return 1;
		case 5:
			ft_printf(BMAG);
			return 1;
		case 6:
			ft_printf(BCYN);
			return 1;
		case 7:
			ft_printf(BREDBRIGHT);
			return 1;
		case 8:
			ft_printf(BGRNBRIGHT);
			return 1;
		case 9:
			ft_printf(BYELBRIGHT);
			return 1;
		default:
			ft_printf("   |");
			return -1;
	}
}

static void	printTeamColor(void)
{
	ft_printf("\n\n --------------");
	for (unsigned int i = 1; i < NB_MAX_TEAMS * 2; i++)
		ft_printf("--");
	ft_printf("\n| Team ID        |");
	for (unsigned int i = 1; i < NB_MAX_TEAMS; i++)
	{
		chooseColor(i);
		ft_printf(" %d " CRESET "|", i);
	}
	ft_printf("\n --------------");
	for (unsigned int i = 1; i < NB_MAX_TEAMS * 2; i++)
		ft_printf("--");
}

static void	printNbTeamPlayer(t_map_info *map)
{
	ft_printf("\n| Players alive  |");
	for (unsigned int i = 1; i < NB_MAX_TEAMS; i++)
		ft_printf(" %d |", map->nb_player_team[i]);
	ft_printf("\n --------------");
	for (unsigned int i = 1; i < NB_MAX_TEAMS * 2; i++)
		ft_printf("--");
	ft_printf("\n");
}

static void	printBoard(t_map_info *map)
{
	ft_printf(TERMINAL_RESET);

	ft_printf(" -");
	for (unsigned int i = 0; i < BOARD_X_MAX * 2 - 1; i++)
		ft_printf("--");

	for (unsigned int y = 0; y < BOARD_Y_MAX; y++)
	{
		ft_printf("\n|");
		for (unsigned int x = 0; x < BOARD_X_MAX; x++)
		{
			if (chooseColor(map->map[x][y]) != -1)
				ft_printf(" %d " CRESET "|", map->map[x][y] + 1);
		}

		ft_printf("\n -");
		for (unsigned int i = 0; i < BOARD_X_MAX * 2 - 1; i++)
			ft_printf("--");
	}
}

static int	printWinner(t_shared_resources *shared_rcs, t_map_info *map)
{
	int	team_id = 0;

	if (semLock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
		return -1;

	for (unsigned int i = 1; i < NB_MAX_TEAMS; i++)
	{
		if (map->nb_player_team[i] != 0)
			team_id = i;
	}

	if (semUnlock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
		return -1;
	
	if (team_id == 0)
	{
		log_err("No one joined the game");
		return -1;
	}

	ft_printf(BWHT "Congratulations to team ");
	chooseColor(team_id);
	ft_printf("%d" CRESET " for winning the game !\n", team_id);

	return team_id;
}

int	graphicMode(t_shared_resources *shared_rcs, t_map_info *map)
{
	if (joinGame(shared_rcs, map, true) == -1)
		return -1;
	if (waitGameStart(shared_rcs, map) == -1)
		return -1;

	unsigned int	teams_still_in_game = 9;
	while (teams_still_in_game >= 2)
	{
		if (gIsSigReceived)
			return -1;
		if (semLock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
			return -1;

		teams_still_in_game = getNbTeamsInGame(map);
		if (map->game_state == STATE_PRINT)
		{
			if (teams_still_in_game >= 2)
			{
				printBoard(map);
				printTeamColor();
				printNbTeamPlayer(map);
			}
			else
				map->game_state = STATE_WON;
		}

		if (semUnlock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
			return -1;

		sleep(TIME_BETWEEN_ACTION);
	}

	if (printWinner(shared_rcs, map) == -1)
		return -1;
	return 1;
}
