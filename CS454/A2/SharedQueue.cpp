#ifndef SHARED_QUEUE
#define SHARED_QUEUE

#include <pthread.h>
#include <queue>

template <typename T>
class SharedQueue
{
private:
	std::queue<T> queue;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

public:
	T& front()
	{
		pthread_mutex_lock(&mutex);
		T res = 
		pthread_mutex_unlock(&mutex);
	}
};

class
#endif