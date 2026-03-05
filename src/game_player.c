#include "game_utils.h"
#include "shared_resources.h"
#include "log.h"
#include "game_private.h"

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

extern volatile __sig_atomic_t	gIsSigReceived;
extern volatile __sig_atomic_t	gIsSemLocked;

static void	unspawn(t_map_info *map, t_player *player)
{
	map->map[player->position.x][player->position.y] = 0;
	map->nb_player_team[player->team - 1] -= 1;
	map->game_state = STATE_PRINT;
}

static int	killPlayer(t_shared_resources *shared_rcs, t_map_info *map, t_player *player)
{
	if (gIsSemLocked == false)
		semLock(shared_rcs->sem_id);
	unspawn(map, player);
	semUnlock(shared_rcs->sem_id);
	return -1;
}

static bool	checkIfPositionIsEnemy(t_map_info *map, int x, int y, unsigned int player_team)
{
	if (x < 0 || x >= BOARD_X_MAX)
		return false;
	if (y < 0 || y >= BOARD_Y_MAX)
		return false;

	if (map->map[x][y] == 0)
		return false;
	if (map->map[x][y] != player_team)
		return true;
	return false;
}

static bool	isPlayerDead(t_shared_resources *shared_rcs, t_map_info *map, t_player *player)
{
	unsigned int	teamsAround[NB_MAX_TEAMS];
	for (unsigned int i = 0; i < NB_MAX_TEAMS; i++)
		teamsAround[i] = 0;

	for (unsigned int x = 0; x < 3; x++)
	{
		for (unsigned int y = 0; y < 3; y++)
		{
			if (checkIfPositionIsEnemy(map, player->position.x - 1 + x, player->position.y - 1 + y, player->team))
			{
				unsigned int	team = map->map[player->position.x - 1 + x][player->position.y - 1 + y];
				teamsAround[team] += 1;
				if (teamsAround[team] >= 2)
				{
					log_info_code("Sorry, you was killed by 2 ennemies from the team ", team);
					killPlayer(shared_rcs, map, player);
					return true;
				}
			}
		}
	}
	return false;
}

static bool	checkIfPositionIsEmpty(t_map_info *map, int x, int y)
{
	if (x < 0 || x >= BOARD_X_MAX)
		return false;
	if (y < 0 || y >= BOARD_Y_MAX)
		return false;

	if (map->map[x][y] == 0)
		return true;
	return false;
}

static int	movePlayer(t_map_info *map, int x, int y, t_player *player)
{
	if (checkIfPositionIsEmpty(map, x, y))
	{
		map->map[x][y] = player->team;
		map->map[player->position.x][player->position.y] = 0;
		player->position.x = x;
		player->position.y = y;

		return 1;
	}
	else
		return -1;
}

static short	countPossibleMove(const bool possibleMove[4])
{
	short	total = 0;
	for (unsigned int i = 0; i < 4; i++)
	{
		if (possibleMove[i])
			total += 1;
	}
	return total;
}

static t_position	getNewPositionFromDirection(t_player *player, int direction)
{
	t_position	movement = { .x = -1, .y = -1};
	switch (direction)
	{
		case 0:
			movement.x = player->position.x;
			movement.y = player->position.y - 1;
			return movement;
		case 1:
			movement.x = player->position.x + 1;
			movement.y = player->position.y;
			return movement;
		case 2:
			movement.x = player->position.x;
			movement.y = player->position.y + 1;
			return movement;
		case 3:
			movement.x = player->position.x - 1;
			movement.y = player->position.y;
			return movement;
		
		default:
			return movement;
	}
}

static t_position	randomMove(t_map_info *map, t_player *player)
{
	bool	possible_move[4];
	possible_move[0] = checkIfPositionIsEmpty(map, player->position.x, player->position.y - 1); // NORTH
	possible_move[1] = checkIfPositionIsEmpty(map, player->position.x + 1, player->position.y); // EAST
	possible_move[2] = checkIfPositionIsEmpty(map, player->position.x, player->position.y + 1); // SOUTH
	possible_move[3] = checkIfPositionIsEmpty(map, player->position.x - 1, player->position.y); // WEST

	unsigned char	r = random() % countPossibleMove(possible_move);
	unsigned char	c = 0;
	for (unsigned int i = 0; i < 4; i++)
	{
		if (possible_move[i])
		{
			if (c == r)
				return getNewPositionFromDirection(player, i);
			c++;
		}
	}
	return getNewPositionFromDirection(player, -1);
}

static t_game_state	move(t_map_info *map, t_player *player)
{
	t_position	random_move = randomMove(map, player);
	if (random_move.x == -1)
		return STATE_ERROR;

	if (movePlayer(map, random_move.x, random_move.y, player) == -1)
		return STATE_ERROR;

	return map->game_state;
}

static int	play(t_shared_resources *shared_rcs, t_map_info *map, t_player *player)
{
	t_game_state	state = STATE_PLAY;
	while (state == STATE_PLAY || state == STATE_PRINT)
	{
		if (gIsSigReceived)
			return killPlayer(shared_rcs, map, player);
		if (semLock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
			return killPlayer(shared_rcs, map, player);
		
		if (isPlayerDead(shared_rcs, map, player))
			state = STATE_DEAD;
		else
			state = move(map, player);

		if (state == STATE_ERROR)
			return killPlayer(shared_rcs, map, player);

		if (semUnlock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
			return killPlayer(shared_rcs, map, player);

		sleep(TIME_BETWEEN_ACTION);
	}

	if (state == STATE_WON)
		log_info("Congratulations, you won the game !");

	return 1;
}

static bool	isSpawnable(t_map_info *map, t_position pos)
{
	if (pos.x < 0 || pos.y < 0)
		return false;
	if (map->map[pos.x][pos.y] == 0)
		return true;
	return false;
}

static int	setPlayer(t_map_info *map, t_player *player, t_position pos)
{
	player->position = pos;
	map->map[pos.x][pos.y] = player->team;
	map->nb_player_team[player->team] += 1;
	map->nb_player += 1;

	return 1;
}

static int	foundSpawnPos(t_map_info *map, t_player *player)
{
	t_position	spawn_pos = { .x = -1, .y = -1};
	int			nb_try = (BOARD_X_MAX * BOARD_Y_MAX) / 2;

	while (isSpawnable(map, spawn_pos) != true && nb_try > 0)
	{
		spawn_pos.x = random() % BOARD_X_MAX;
		spawn_pos.y = random() % BOARD_Y_MAX;
		nb_try--;
	}

	if (isSpawnable(map, spawn_pos) == true)
		return setPlayer(map, player, spawn_pos);
	else
	{
		for (unsigned int x = 0; x < BOARD_X_MAX; x++)
		{
			for (unsigned int y = 0; y < BOARD_Y_MAX; y++)
			{
				spawn_pos = (t_position){ .x = x, .y = y };
				if (isSpawnable(map, spawn_pos) == true)
					return setPlayer(map, player, spawn_pos);
			}
		}
	}
	return -1;
}

static int	spawnPlayer(t_shared_resources *shared_rcs, t_map_info *map, t_player *player)
{
	if (semLock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
		return -1;

	if (foundSpawnPos(map, player) == -1)
	{
		log_err("There is no suitable spawn position");
		semUnlock(shared_rcs->sem_id);
		return -1;
	}

	if (semUnlock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
		return -1;
	return 0;
}

int	playerMode(t_shared_resources *shared_rcs, t_map_info *map, t_player *player)
{
	log_info("Player mode started");

	if (joinGame(shared_rcs, map, false) == -1)
		return -1;
	if (spawnPlayer(shared_rcs, map, player) == -1)
		return -1;

	if (waitGameStart(shared_rcs, map) == -1)
		return killPlayer(shared_rcs, map, player);

	return (play(shared_rcs, map, player));
}
