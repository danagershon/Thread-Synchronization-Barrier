#include "Barrier.h"
#include "pthread.h"

Barrier::Barrier(unsigned int num_of_threads)
    : N(num_of_threads), arrived_threads_count(0)
{
    pthread_mutex_init(&mutex, NULL);
    sem_init(&barrier_entrance, 0, 0);
    sem_init(&barrier_exit, 0, 0);
}

Barrier::~Barrier() {
    pthread_mutex_destroy(&mutex);
    sem_destroy(&barrier_entrance);
    sem_destroy(&barrier_exit);
}

void Barrier::wait() {
    enter_barrier();
    exit_barrier();
}

void Barrier::enter_barrier() {
    pthread_mutex_lock(&mutex);
    arrived_threads_count++;
    if (arrived_threads_count == N) {
        for (int i = 0; i < N; i++) {
            sem_post(&barrier_entrance);
        }
    }
    pthread_mutex_unlock(&mutex);
    sem_wait(&barrier_entrance);
}

void Barrier::exit_barrier() {
    pthread_mutex_lock(&mutex);
    arrived_threads_count--;
    if (arrived_threads_count == 0) {
        for (int i = 0; i < N; i++) {
            sem_post(&barrier_exit);
        }
    }
    pthread_mutex_unlock(&mutex);
    sem_wait(&barrier_exit);
}
