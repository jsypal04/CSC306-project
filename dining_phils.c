#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

// Declarations

// This type represents the state of a given philosopher
typedef enum {
    THINKING,
    HUNGRY,
    EATING
} state_t;

uint8_t philosophers_eating = 0; // This variable is used to track the number of philosophers currently eating

// Mutices to lock different resources/operations
// There are a total of 7 mutices, 1 for each chopstick plus one for the pickup operation and one to update the counter
pthread_mutex_t pickup_chopsticks_mutex;
pthread_mutex_t counter_update;
pthread_mutex_t chopstick_mutexes[5];


// Function to delay a thread for a given number of seconds
void delay_s(int delay) {
    time_t start_time, current_time;
    start_time = time(NULL);

    do {
        current_time = time(NULL);
    } while (current_time - start_time < delay);
}

// This is the function that will run for each concurrent philosopher thread
void* philosopher_process(void* arg) {
    int64_t id = (int64_t) arg; // get the thread id
    state_t state = THINKING; // Inialize the state
    
    while (1) {
        // If the philosopher is thinking, delay for a duration based on the thread id
        // This ensures a variation in the time that different threads request different resources
        if (state == THINKING) {
            delay_s(id + 1);
            state = HUNGRY;
        }

        // Aquire lock on pickup_chopsticks()
        // This means only one philosopher can pickup chopsticks at a given time regardless of where they are seated
        pthread_mutex_lock(&pickup_chopsticks_mutex);

        // Only allow a thread to lock both adjacent chopsticks (not one)
        // The pthread_mutex_trylock() function trys to lock the mutex but if it fails it does not block but it returns immediately
        if (pthread_mutex_trylock(&chopstick_mutexes[id]) != 0) {
            // If locking fails, release the pickup chopsticks lock
            pthread_mutex_unlock(&pickup_chopsticks_mutex);
            continue;
        }
        // Try to pick up the second chopstick
        if (pthread_mutex_trylock(&chopstick_mutexes[(id + 1) % 5]) != 0) {
            // If locking fails, release lock on first chopstick and pickup chopsticks lock
            pthread_mutex_unlock(&chopstick_mutexes[id]);
            pthread_mutex_unlock(&pickup_chopsticks_mutex);
            continue;
        }
        
        // Release lock on pickup_chopsticks()
        pthread_mutex_unlock(&pickup_chopsticks_mutex);

        // At this point in the loop, the current thread has locked both adjacent chopsticks
        state = EATING;
        time_t start = time(NULL);
        
        // increment the number of philosophers that are eating
        pthread_mutex_lock(&counter_update);
        philosophers_eating++;
        pthread_mutex_unlock(&counter_update);
        printf("Philosopher %ld is EATING\n", id); // print a status message


        // the thread then delays (same as above) before returning to THINKING
        delay_s(id + 1);
        state = THINKING;
        // Release the locks on the chopsticks
        pthread_mutex_unlock(&chopstick_mutexes[id]);
        pthread_mutex_unlock(&chopstick_mutexes[(id + 1) % 5]);

        // Decrement the counter
        pthread_mutex_lock(&counter_update);
        philosophers_eating--;
        pthread_mutex_unlock(&counter_update);

    }

    return NULL;
}

// This function is used in a thread running concurrently with the philosophers
// Its job is to print the current number of philosophers eating once per second
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
    // Declare variables
    pthread_t phil_1, phil_2, phil_3, phil_4, phil_5;
    pthread_t print_thread;
    int64_t id1, id2, id3, id4, id5;

    // Initialize ids
    id1 = 0;
    id2 = 1;
    id3 = 2;
    id4 = 3;
    id5 = 4;

    // create threads
    pthread_create(&phil_1, NULL, philosopher_process, (void*)id1);
    pthread_create(&phil_2, NULL, philosopher_process, (void*)id2);
    pthread_create(&phil_3, NULL, philosopher_process, (void*)id3);
    pthread_create(&phil_4, NULL, philosopher_process, (void*)id4);
    pthread_create(&phil_5, NULL, philosopher_process, (void*)id5);
    pthread_create(&print_thread, NULL, print_counter, NULL);

    // wait for threads
    pthread_join(phil_1, NULL);
    pthread_join(phil_2, NULL);
    pthread_join(phil_3, NULL);
    pthread_join(phil_4, NULL);
    pthread_join(phil_5, NULL);
    pthread_join(print_thread, NULL);

    return EXIT_SUCCESS;
}
