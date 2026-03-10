#pragma once

#include <time.h>
#include <stdbool.h>

#define TIME_BETWEEN_ACTION 1
#define TIME_BEFORE_STARTING_GAME 20

// NB_MAX_TEAMS should be the number maximum of teams + 1
#define NB_MAX_TEAMS 10

#define BOARD_X_MAX 15
#define BOARD_Y_MAX 15

typedef enum e_game_state {
	STATE_PLAY,
	STATE_PRINT,
	STATE_DEAD,
	STATE_WON,
	STATE_ERROR
} t_game_state;

typedef struct	s_map_info {
	struct timespec	start_time;
	unsigned int	nb_player;
	unsigned int	nb_player_team[NB_MAX_TEAMS];
	unsigned int	map[BOARD_X_MAX][BOARD_Y_MAX];
	t_game_state	game_state;
	bool			graphic_on;
} t_map_info;

typedef	struct	s_position {
	int	x;
	int	y;
} t_position;

typedef struct	s_player {
	unsigned int	team;
	t_position		position;
} t_player;
