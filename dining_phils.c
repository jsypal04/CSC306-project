#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* thread(void* arg) {
    printf("Running thread %d.\n", (int)(long)arg);

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
