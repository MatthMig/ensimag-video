#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int counter;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} CounterMonitor;

CounterMonitor monitor;

void CounterMonitor_init(CounterMonitor *monitor) {
    monitor->counter = 0;
    pthread_mutex_init(&monitor->mutex, NULL);
    pthread_cond_init(&monitor->cond, NULL);
}

void CounterMonitor_destroy(CounterMonitor *monitor) {
    pthread_mutex_destroy(&monitor->mutex);
    pthread_cond_destroy(&monitor->cond);
}

void incrementCounter(void *data) {
    CounterMonitor *monitor = (CounterMonitor *)data;
    pthread_mutex_lock(&monitor->mutex);
    monitor->counter++;
    if (monitor->counter == 10) {
        pthread_cond_signal(&monitor->cond);
    }
    pthread_mutex_unlock(&monitor->mutex);
}

void waitForCounter10(void *data) {
    CounterMonitor *monitor = (CounterMonitor *)data;
    pthread_mutex_lock(&monitor->mutex);
    while (monitor->counter < 10) {
        pthread_cond_wait(&monitor->cond, &monitor->mutex);
    }
    pthread_mutex_unlock(&monitor->mutex);
}

void* print_hello(void* data) {
    incrementCounter(data);
    printf("Hello, World!\n");
    return NULL;
}

void* print_done(void* data) {
    waitForCounter10(data);
    printf("Done!\n");
    return NULL;
}

int main() {
    CounterMonitor_init(&monitor);

    pthread_t threads[11];
    int rc;
    for (int i = 0; i < 10; i++) {
        rc = pthread_create(&threads[i], NULL, print_hello, (void *)&monitor);
        if (rc) {
            printf("Error: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    rc = pthread_create(&threads[10], NULL, print_done, (void *)&monitor);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    // Join all threads
    for (int i = 0; i < 11; i++) {
        rc = pthread_join(threads[i], NULL);
        if (rc) {
            printf("Error: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

    // Destroy the CounterMonitor
    CounterMonitor_destroy(&monitor);

    return 0;
}
