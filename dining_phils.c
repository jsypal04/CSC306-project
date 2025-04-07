#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
    EATING,
    THINKING
} State;

pthread_mutex_t chopstick_mutex;

// Delays the current thread for `delay` seconds
void delay_s(int delay) {

    time_t start_time, current_time;
    start_time = time(NULL);

    do {
        current_time = time(NULL);
    } while (current_time - start_time < delay);
}


void* thread(void* arg) {
    int id = (int)(long)arg;
    State state = THINKING;

    while (1) {
        if (state == EATING) {
            pthread_mutex_lock(&chopstick_mutex);
            printf("Thread %d is eating\n", id);
            delay_s(id);
            pthread_mutex_unlock(&chopstick_mutex);
            state = THINKING;
            printf("Thread %d is thinking\n", id);
        }
        else {
            delay_s(1);
            state = EATING;
        }
    }

    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    int id1 = 1;
    int id2 = 2;

    pthread_create(&thread1, NULL, thread, (void*)(long)id1);
    pthread_create(&thread2, NULL, thread, (void*)(long)id2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Main thread. Both threads finished\n");
    

    return EXIT_SUCCESS;
}
