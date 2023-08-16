#ifndef BARRIER_H_
#define BARRIER_H_

#include <semaphore.h>

class Barrier {
public:
    // constructor
    explicit Barrier(unsigned int num_of_threads);

    void wait();

    // destructor
    ~Barrier();

private:
    int N; // number of threads
    int arrived_threads_count;
    pthread_mutex_t mutex; // for accessing N and arrived_threads_count
    sem_t barrier_entrance;
    sem_t barrier_exit;

    void enter_barrier();

    void exit_barrier();
};

#endif // BARRIER_H_


