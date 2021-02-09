#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>

#define TWEET_SIZE 144

sem_t rw_mutex;
sem_t mutex;
sem_t full;
int read_count = 0;

char tweet[TWEET_SIZE];
int fd = STDIN_FILENO;
int num_readers = 0;

int read_line()
{
    int n = 0;
    char c = '\0';

    do {
        if (read(fd, &c, sizeof(char)) != sizeof(char))
            return -1;

        tweet[n++] = c;
    } while (n < TWEET_SIZE && c != '\n');

    if (n > 1 && tweet[n - 2] == '\r')
        tweet[--n] = '\0';

    tweet[--n] = '\0';
    return 0;
}

void *
reader(void* v)
{
    while (1) {
        sem_wait(&full);

        sem_wait(&mutex);
        read_count++;
        if (read_count == 1)
            sem_wait(&rw_mutex);
        sem_post(&mutex);

        printf("%s\n", tweet);

        sem_wait(&mutex);
        read_count--;
        if (read_count == 0)
            sem_post(&rw_mutex);
        sem_post(&mutex);
    }
}


void writer(void* v)
{
    while (1) {

        sem_wait(&rw_mutex);

        printf("Whats on your mind?: ");
        fflush(stdout);
        if (read_line() == -1) exit(0);

        sem_post(&rw_mutex);

        for (int i = 0; i < num_readers; i++)
            sem_post(&full);
    }
}

int main()
{
    pthread_t thread;

    sem_init(&rw_mutex, 0, 1);
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);

    printf("How many friends do you have: ");
    fflush(stdout);

    read_line();
    num_readers = strtol((char*) &tweet, NULL, 10);

    for (int i = 0; i < num_readers; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, reader, NULL);
    }

    writer(NULL);
}
