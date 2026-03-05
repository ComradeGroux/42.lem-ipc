#pragma once

#include "game_utils.h"
#include "shared_resources.h"

#include <stdbool.h>

int	joinGame(t_shared_resources *shared_rcs, t_map_info *map, bool graphic_mode);
int	waitGameStart(t_shared_resources *shared_rcs, t_map_info *map);
