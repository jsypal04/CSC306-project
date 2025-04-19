#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef enum {
    THINKING,
    HUNGRY,
    EATING
} state_t;

uint8_t philosophers_eating = 0;
pthread_mutex_t pickup_chopsticks_mutex;
pthread_mutex_t counter_update;
pthread_mutex_t chopstick_mutexes[5];

void delay_s(int delay) {
    time_t start_time, current_time;
    start_time = time(NULL);

    do {
        current_time = time(NULL);
    } while (current_time - start_time < delay);
}

void* philosopher_process(void* arg) {
    int64_t id = (int64_t) arg;
    state_t state;

    uint8_t print_flag = 0;

    while (1) {
        if (state == THINKING) {
            // printf("Philosopher %ld is THINKING\n", id);
            delay_s(id + 1);
            state = HUNGRY;
        }

        // Aquire lock on pickup_chopsticks()
        pthread_mutex_lock(&pickup_chopsticks_mutex);

        // Only allow a thread to lock both adjacent chopsticks (not one)
        if (pthread_mutex_trylock(&chopstick_mutexes[id]) != 0) {
            pthread_mutex_unlock(&pickup_chopsticks_mutex);
            continue;
        }
        if (pthread_mutex_trylock(&chopstick_mutexes[(id + 1) % 5]) != 0) {
            pthread_mutex_unlock(&chopstick_mutexes[id]);
            pthread_mutex_unlock(&pickup_chopsticks_mutex);
            continue;
        }
        state = EATING;
        pthread_mutex_lock(&counter_update);
        philosophers_eating++;
        pthread_mutex_unlock(&counter_update);
        printf("Philosopher %ld is EATING\n", id);

        // Release lock on pickup_chopsticks()
        pthread_mutex_unlock(&pickup_chopsticks_mutex);

        delay_s(id + 1);
        state = THINKING;
        pthread_mutex_lock(&counter_update);
        philosophers_eating--;
        pthread_mutex_unlock(&counter_update);

        pthread_mutex_unlock(&chopstick_mutexes[id]);
        pthread_mutex_unlock(&chopstick_mutexes[(id + 1) % 5]);
    }

    return NULL;
}

void* print_counter(void* arg) {

    while (1) {
        pthread_mutex_lock(&counter_update);
        printf("Philosophers Currently Eating: %d\n", philosophers_eating);
        pthread_mutex_unlock(&counter_update);

        delay_s(1);
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t phil_1, phil_2, phil_3, phil_4, phil_5;
    pthread_t print_thread;
    int64_t id1, id2, id3, id4, id5;

    id1 = 0;
    id2 = 1;
    id3 = 2;
    id4 = 3;
    id5 = 4;

    pthread_create(&phil_1, NULL, philosopher_process, (void*)id1);
    pthread_create(&phil_2, NULL, philosopher_process, (void*)id2);
    pthread_create(&phil_3, NULL, philosopher_process, (void*)id3);
    pthread_create(&phil_4, NULL, philosopher_process, (void*)id4);
    pthread_create(&phil_5, NULL, philosopher_process, (void*)id5);
    pthread_create(&print_thread, NULL, print_counter, NULL);

    pthread_join(phil_1, NULL);
    pthread_join(phil_2, NULL);
    pthread_join(phil_3, NULL);
    pthread_join(phil_4, NULL);
    pthread_join(phil_5, NULL);
    pthread_join(print_thread, NULL);

    return EXIT_SUCCESS;
}
