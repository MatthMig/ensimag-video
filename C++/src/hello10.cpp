#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

class CounterMonitor {
private:
    int counter;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

public:
    CounterMonitor() : counter(0) {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
    }

    ~CounterMonitor() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    void incrementCounter() {
        pthread_mutex_lock(&mutex);
        counter++;
        if (counter == 10) {
            pthread_cond_signal(&cond); // Signal waiting threads when counter reaches 10
        }
        pthread_mutex_unlock(&mutex);
    }

    void waitForCounter10() {
        pthread_mutex_lock(&mutex);
        while (counter < 10) {
            pthread_cond_wait(&cond, &mutex); // Wait until counter reaches 10
        }
        pthread_mutex_unlock(&mutex);
    }
};

CounterMonitor monitor;

void* print_hello(void* arg) {
    monitor.incrementCounter(); 
    cout << "Hello, World!" << endl;
    return NULL;
}

void* print_done(void* arg) {
    monitor.waitForCounter10();
    cout << "Done!" << endl;
    return NULL;
}

int main() {
    pthread_t threads[11];
    int rc;

    // Launch one thread printing "Done"
    rc = pthread_create(&threads[10], NULL, print_done, NULL);
    if (rc) {
        cerr << "Error: Unable to create thread, " << rc << endl;
        exit(-1);
    }

    // Launch 10 threads printing "Hello"
    for (int i = 0; i < 10; ++i) {
        rc = pthread_create(&threads[i], NULL, print_hello, NULL);
        if (rc) {
            cerr << "Error: Unable to create thread, " << rc << endl;
            exit(-1);
        }
    }

    // Wait for the completion of all threads
    for (int i = 0; i < 11; ++i) {
        rc = pthread_join(threads[i], NULL);
        if (rc) {
            cerr << "Error: Unable to join thread, " << rc << endl;
            exit(-1);
        }
    }

    return 0;
}
