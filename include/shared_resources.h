#pragma once

#include "game_utils.h"

#include <sys/ipc.h>
#include <stddef.h>

#define IPC_RESULT_ERROR -1

typedef struct s_shared_resources {
	int		shm_id;
	void	*shm_addr;
	size_t	shm_size;
	int		sem_id;
	int		msg_id;
} t_shared_resources;

typedef enum e_clean_shared {
	CLEAN_ALL,
	CLEAN_SEM,
	CLEAN_SHM
} t_clean_shared;

key_t	generateSysVKey(int i);
int		cleanSharedResources(t_shared_resources *shared_rcs, t_clean_shared flag);
int		getSharedResources(t_shared_resources *shared_rcs, key_t key);
int		initSharedResources(t_shared_resources *shared_rcs, t_map_info *map);
