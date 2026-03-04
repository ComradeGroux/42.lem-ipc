#include "shared_resources.h"
#include "log.h"

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#include <unistd.h>

key_t	generateSysVKey(int i)
{
	key_t	key;
	char	buff[1024];

	if (readlink("/proc/self/exe", buff, sizeof(buff)) == IPC_RESULT_ERROR)
	{
		log_syserr("(readlink)");
		return -1;
	}

	key = ftok(buff, i);
	if (key == -1)
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

static int	cleanMsg(int msg_id)
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
				ret += cleanMsg(shared_rcs->msg_id);
				/* fallthrough */
			case CLEAN_SEM:
				ret += cleanSem(shared_rcs->sem_id);
				/* fallthrough */
			case CLEAN_SHM:
				ret += cleanShm(shared_rcs->shm_id);
				break;
			default:
				return IPC_RESULT_ERROR;
		}
	}
	return ret;
}

int	getSharedResources(t_shared_resources *shared_rcs, key_t key)
{
	shared_rcs->shm_id = shmget(key, sizeof(t_shared_resources), IPC_CREAT | 0600);
	if (shared_rcs->shm_id == IPC_RESULT_ERROR)
	{
		log_syserr("(shmget)");
		return IPC_RESULT_ERROR;
	}
	shared_rcs->shm_addr = shmat(shared_rcs->shm_id, NULL, 0);
	if (shared_rcs->shm_addr == NULL)
	{
		log_syserr("(shmat)");
		cleanSharedResources(shared_rcs, CLEAN_SHM);
		return IPC_RESULT_ERROR;
	}

	shared_rcs->sem_id = semget(key, 1, IPC_CREAT | 0600);
	if (shared_rcs->sem_id == IPC_RESULT_ERROR)
	{
		log_syserr("(semget)");
		cleanSharedResources(shared_rcs, CLEAN_SEM);
		return IPC_RESULT_ERROR;
	}

	shared_rcs->msg_id = msgget(key, IPC_CREAT | 0600);
	if (shared_rcs->msg_id == IPC_RESULT_ERROR)
	{
		log_syserr("(msgget)");
		cleanSharedResources(shared_rcs, CLEAN_ALL);
		return IPC_RESULT_ERROR;
	}

	return 0;
}

int	initSharedResources(t_shared_resources *shared_rcs, t_map_info *map)
{
	(void)shared_rcs;
	(void)map;

	return 0;
}
