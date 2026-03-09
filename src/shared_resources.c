#include "shared_resources.h"
#include "log.h"

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

extern volatile __sig_atomic_t	gIsSigReceived;
extern volatile __sig_atomic_t	gIsSemLocked;

key_t	generateSysVKey(int i)
{
	key_t	key;
	char	buff[1024];

	ssize_t	res = readlink("/proc/self/exe", buff, sizeof(buff));
	if (res == IPC_RESULT_ERROR || res >= 1024)
	{
		log_syserr("(readlink)");
		return IPC_RESULT_ERROR;
	}
	buff[res] = '\0';

	key = ftok(buff, i);
	if (key == IPC_RESULT_ERROR)
		log_syserr("(ftok)");
	return key;
}

static int	cleanShm(int shm_id)
{
	int	ret = shmctl(shm_id, IPC_RMID, NULL);
	if (ret != IPC_RESULT_ERROR)
		log_verb("Shared memory segment marked for destroy");
	return ret;
}

static int	cleanSem(int sem_id)
{
	int	ret = semctl(sem_id, 0, IPC_RMID);
	if (ret != IPC_RESULT_ERROR)
		log_verb("Semaphore set removed");
	return ret;
}

int	cleanMsg(int msg_id)
{
	int	ret = msgctl(msg_id, IPC_RMID, NULL);
	if (ret != IPC_RESULT_ERROR)
		log_verb("Message queue removed");
	return ret;
}

static int	getNbShmAttached(int shm_id)
{
	struct shmid_ds	buff = {};

	if (shmctl(shm_id, IPC_STAT, &buff) == IPC_RESULT_ERROR)
	{
		log_syserr("(shmctl - IPC_STAT)");
		return IPC_RESULT_ERROR;
	}
	return buff.shm_nattch;
}

int	cleanSharedResources(t_shared_resources *shared_rcs, t_clean_shared flag)
{
	int	nb_attach = 0;
	int	ret = 0;

	if (shmdt(shared_rcs->shm_addr) == IPC_RESULT_ERROR)
	{
		log_syserr("(smhdt)");
		ret--;
	}
	nb_attach = getNbShmAttached(shared_rcs->shm_id);
	if (nb_attach == IPC_RESULT_ERROR)
		return IPC_RESULT_ERROR;

	if (nb_attach == 0)
	{
		switch (flag)
		{
			case CLEAN_ALL:
				// ret += cleanMsg(player->msg_id);
				/* fallthrough */
			case CLEAN_FROM_SEM:
				ret += cleanSem(shared_rcs->sem_id);
				/* fallthrough */
			case CLEAN_FROM_SHM:
				ret += cleanShm(shared_rcs->shm_id);
				break;
			default:
				return IPC_RESULT_ERROR;
		}
	}
	return ret;
}

int	getSharedResources(t_shared_resources *shared_rcs, t_player *player, key_t key)
{
	shared_rcs->shm_id = shmget(key, sizeof(t_map_info), IPC_CREAT | 0600);
	if (shared_rcs->shm_id == IPC_RESULT_ERROR)
	{
		log_syserr("(shmget)");
		return IPC_RESULT_ERROR;
	}
	shared_rcs->shm_addr = shmat(shared_rcs->shm_id, NULL, 0);
	if (shared_rcs->shm_addr == (void *)IPC_RESULT_ERROR)
	{
		log_syserr("(shmat)");
		cleanSharedResources(shared_rcs, CLEAN_FROM_SHM);
		return IPC_RESULT_ERROR;
	}

	shared_rcs->sem_id = semget(key, 1, IPC_CREAT | 0600);
	if (shared_rcs->sem_id == IPC_RESULT_ERROR)
	{
		log_syserr("(semget)");
		cleanSharedResources(shared_rcs, CLEAN_FROM_SEM);
		return IPC_RESULT_ERROR;
	}

	if (player->team == 0)
		return 0;
	player->msg_id = msgget(generateSysVKey(player->team), IPC_CREAT | 0600);
	if (player->msg_id == IPC_RESULT_ERROR)
	{
		log_syserr("(msgget)");
		cleanMsg(player->msg_id);
		cleanSharedResources(shared_rcs, CLEAN_ALL);
		return IPC_RESULT_ERROR;
	}

	return 0;
}

static int	initSharedMemory(t_map_info *map)
{
	struct timespec	t;
	if (clock_gettime(CLOCK_REALTIME, &t) == -1)
	{
		log_syserr("(clock_gettime)");
		return IPC_RESULT_ERROR;
	}

	map->start_time = t;
	map->graphic_on = false;
	map->game_state = STATE_PLAY;

	return 0;
}

int	initSharedResources(t_shared_resources *shared_rcs, t_map_info **map)
{
	struct semid_ds	info;
	union semun {
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	} arg = { .buf = &info };

	if (semctl(shared_rcs->sem_id, 0, IPC_STAT, arg) == IPC_RESULT_ERROR)
	{
		log_syserr("(semctl - IPC_STAT)");
		return IPC_RESULT_ERROR;
	}

	*map = shared_rcs->shm_addr;
	if (info.sem_otime == 0)
	{
		arg.val = 1;
		if (semctl(shared_rcs->sem_id, 0, SETVAL, arg) == IPC_RESULT_ERROR)
		{
			log_syserr("(semctl - SETVAL)");
			return IPC_RESULT_ERROR;
		}

		struct sembuf compete = { .sem_num = 0, .sem_op = -1, .sem_flg = 0 };
		if (semop(shared_rcs->sem_id, &compete, 1) == 0)
		{
			gIsSemLocked = true;
			log_verb("Creator: Initializing of shared resources");

			if (initSharedMemory(*map) == IPC_RESULT_ERROR)
			{
				semUnlock(shared_rcs->sem_id);
				return IPC_RESULT_ERROR;
			}

			if (semUnlock(shared_rcs->sem_id) == IPC_RESULT_ERROR)
				return IPC_RESULT_ERROR;

			log_verb("Creator: Shared resources initialized");
		}
		else if (errno == EAGAIN)
		{
			log_verb("Lost the race, waiting for the creator to finish...");

			struct sembuf wait[2] = {
				{ .sem_num = 0, .sem_op = -1, .sem_flg = 0 },
				{ .sem_num = 0, .sem_op =  1, .sem_flg = 0 }
			};
			if (semop(shared_rcs->sem_id, wait, 2) == IPC_RESULT_ERROR)
			{
				log_syserr("(semop - wait for init)");
				return IPC_RESULT_ERROR;
			}
		}
		else
		{
			log_syserr("(semop - compete)");
			return IPC_RESULT_ERROR;
		}
	}
	else
	{
		struct sembuf wait[2] = {
			{ .sem_num = 0, .sem_op = -1, .sem_flg = 0 },
			{ .sem_num = 0, .sem_op =  1, .sem_flg = 0 }
		};
		if (semop(shared_rcs->sem_id, wait, 2) == IPC_RESULT_ERROR)
		{
			log_syserr("(semop - wait for init)");
			return IPC_RESULT_ERROR;
		}

		log_verb("Not creator: Resources already initialized");
	}

	return 0;
}

int	semLock(int sem_id)
{
	struct sembuf post = { .sem_num = 0, .sem_op = -1, .sem_flg = 0 };
	if (semop(sem_id, &post, 1) == IPC_RESULT_ERROR)
	{
		log_syserr("(semop - lock)");
		return IPC_RESULT_ERROR;
	}
	gIsSemLocked = true;
	return 0;
}

int	semUnlock(int sem_id)
{
	struct sembuf post = { .sem_num = 0, .sem_op = 1, .sem_flg = 0 };
	if (semop(sem_id, &post, 1) == IPC_RESULT_ERROR)
	{
		log_syserr("(semop - unlock)");
		return IPC_RESULT_ERROR;
	}
	gIsSemLocked = false;
	return 0;
}
