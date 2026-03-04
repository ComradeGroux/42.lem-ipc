#pragma once

#include <time.h>

#define NB_MAX_TEAMS 10

#define BOARD_X_MAX 15
#define BOARD_Y_MAX 15

typedef struct	s_map_info {
	struct timespec	start_time;
	struct timespec	time_last_move;
	unsigned int	nb_player;
	unsigned int	nb_player_team[NB_MAX_TEAMS];
	unsigned int	map[BOARD_X_MAX][BOARD_Y_MAX];
} t_map_info;

typedef struct	s_player {
	unsigned int	team;
} t_player;