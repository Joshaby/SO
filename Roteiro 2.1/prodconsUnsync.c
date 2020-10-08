/*
 * Producer/Consumer demo using POSIX threads without synchronization
 * Linux version
 * MJB Apr'05
 */
# include <pthread.h>
# include <stdlib.h>
# include <stdio.h>
# define BUFFER_SIZE 5

/* buffer using a shared integer variable */
typedef struct {
  int writeable; /*true/false*/
  int sharedData;
} buffer;

buffer theBuffer; /* global */
pthread_mutex_t mutex;
pthread_mutex_t mutex1;
pthread_cond_t cond;

int store(buffer *pb) {
  pthread_mutex_lock(&mutex);
  if (pb->sharedData == BUFFER_SIZE) {
    printf("Buffer cheio\n");
    pthread_cond_wait(&cond, &mutex);
  }
  else {
    pb->sharedData += 1;      /*put data in buffer... */
    printf("Stored: %d\n", pb->sharedData);
    pb->writeable = !pb->writeable;
  }
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
  return 0;
}

int retrieve(buffer *pb) {
  pthread_mutex_lock(&mutex);
  int data;
  if (pb->sharedData == 0) {
    printf("Buffer vazio\n");
    pthread_cond_wait(&cond, &mutex);
  }
  else {
    data = pb->sharedData;       /*get data from buffer...*/
    printf("Retrieved: %d\n", data);
    pb->sharedData--;
    pb->writeable = !pb->writeable;
  }
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
  return 0;
}

void delay(int secs) { /*utility function*/
  time_t beg = time(NULL), end = beg + secs;
  do {
    ;
  } while (time(NULL) < end);
}

/* core routine for the producer thread */
void *producer(void *n) {
  while (1) {
    store(&theBuffer);
    delay(rand() % 6);  /* up to 5 sec */
  }
  return n;
}

/* core routine for the consumer thread */
void *consumer(void *n) {
  while (1) {
    retrieve(&theBuffer);
    delay(rand() % 6);  /* up to 5 sec */
  }
  return n;
}

int main() {
  pthread_attr_t *attr = NULL;
  pthread_t prodThread, consThread;

  theBuffer.writeable = 1;
  theBuffer.sharedData = 0;

  srand(time(NULL)); /* initialise the rng */

  pthread_create(&prodThread, attr, producer, NULL);
  pthread_create(&consThread, attr, consumer, NULL);

  pthread_join(prodThread, NULL);
  pthread_join(consThread, NULL);

  printf("Process finished!!");

  return 0;  /*!!!*/
}
