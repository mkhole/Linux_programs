#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/wait.h>

#define MAP_SIZE 1024
#define SHM_NAME "shm_test"

struct memmap {
	pthread_mutex_t lock;
	int counter;
};

struct memmap *map;

static void calculation()
{
	for (int i = 0; i < 10000000; i++) {
		pthread_mutex_lock(&map->lock);
		map->counter++;
		pthread_mutex_unlock(&map->lock);
	}
}

static struct memmap *shmem_init()
{
	int fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
	if (fd < 0)
		return NULL;

	if (ftruncate(fd, MAP_SIZE) < 0)
		return NULL;

	void *mem = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mem == MAP_FAILED)
		return NULL;

	memset(mem, 0, sizeof(struct memmap));
	return (struct memmap *)mem;
}

static void shmem_destory()
{
	if (munmap(map, MAP_SIZE) < 0)
		perror("munmap");

	if (shm_unlink(SHM_NAME) < 0)
		perror("shm_unlink");
}

static void lock_init(pthread_mutex_t *mutex)
{
	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);

	pthread_mutex_init(mutex, &mattr);
}

static void lock_destory(pthread_mutex_t *mutex)
{
	pthread_mutex_destroy(mutex);
}

int main()
{
	map = shmem_init();
	if (!map)
		return -1;

	lock_init(&map->lock);

	if (!fork()) {
		calculation();
		exit(0);
	}

	calculation();
	wait(NULL);

	printf("tatol: %d\n", map->counter);

	lock_destory(&map->lock);
	shmem_destory();

	return 0;
}

