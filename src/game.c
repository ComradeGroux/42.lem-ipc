#include "game_utils.h"
#include "game_mode.h"
#include "shared_resources.h"
#include "log.h"
#include "libft.h"
#include "ANSI-color-codes.h"
#include "game_private.h"

#include <time.h>

extern volatile __sig_atomic_t	gIsSigReceived;
extern volatile __sig_atomic_t	gIsSemLocked;

int	joinGame(t_shared_resources *shared_rcs, t_map_info *map, bool graphic_mode)
{
	if (graphic_mode == true)
	{
		if (semLock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
			return -1;
		if (map->graphic_on == true)
		{
			log_err("There is already a graphical process that was launched");
			semUnlock(shared_rcs->sem_id);
			return -1;
		}
		else
			map->graphic_on = true;
		if (semUnlock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
			return -1;
	}

	struct timespec now;
	if (clock_gettime(CLOCK_REALTIME, &now) == -1)
	{
		log_syserr("(clock_gettime - CLOCK_REALTIME)");
		return -1;
	}
	if (semLock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
		return -1;
	double diff_time = difftime(now.tv_sec, map->start_time.tv_sec);
	if (semUnlock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
		return -1;
	if (diff_time > TIME_BEFORE_STARTING_GAME)
	{
		log_err("The game already started");
		return -1;
	}

	return 0;
}

static unsigned int	getMaxPlayerByTeam(t_map_info *map)
{
	unsigned int	maxPlayer = 0;
	for (unsigned int i = 1; i < NB_MAX_TEAMS; i++)
	{
		if (map->nb_player_team[i] > maxPlayer)
			maxPlayer = map->nb_player_team[i];
	}
	return maxPlayer;
}

int	waitGameStart(t_shared_resources *shared_rcs, t_map_info *map)
{
	struct timespec now;
	if (clock_gettime(CLOCK_REALTIME, &now) == -1)
	{
		log_syserr("(clock_gettime - CLOCK_REALTIME)");
		return -1;
	}

	if (semLock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
		return -1;
	struct timespec start_time = map->start_time;
	if (semUnlock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
		return -1;

	log_info("Waiting for players to join the game");
	ft_printf(BBLU "[  INFO   ]" CRESET " The game will start in ");
	while ((int)difftime(now.tv_sec, start_time.tv_sec) < TIME_BEFORE_STARTING_GAME && gIsSigReceived != true)
	{
		ft_printf("%d... ", TIME_BEFORE_STARTING_GAME - (int)difftime(now.tv_sec, start_time.tv_sec));

		sleep(1);
		if (clock_gettime(CLOCK_REALTIME, &now) == -1)
		{
			log_syserr("(clock_gettime - CLOCK_REALTIME)");
			return -1;
		}
	}
	ft_printf("\n");

	if (semLock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
		return -1;
	if (map->graphic_on == false)
	{
		log_err("There isn't any graphical process launched");
		semUnlock(shared_rcs->sem_id);
		return -1;
	}
	if (getNbTeamsInGame(map) < 2)
	{
		log_err("There isn't enough team to start the game");
		semUnlock(shared_rcs->sem_id);
		return -1;
	}
	if (getMaxPlayerByTeam(map) < 2)
	{
		log_err("There is no team having at least 2 players");
		semUnlock(shared_rcs->sem_id);
		return -1;
	}
	if (semUnlock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
		return -1;

	log_info("Launching the game !");
	return 0;
}

int	getNbTeamsInGame(t_map_info *map)
{
	int	total = 0;
	for (unsigned int i = 1; i < NB_MAX_TEAMS; i++)
	{
		if (map->nb_player_team[i] > 0)
			total += 1;
	}
	return total;
}
