# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <semaphore.h>
# define MAX_CHAIRS 5

typedef struct {
    int barberStatus;
    int queueSize;
    int clientChair;
} barberShop;

sem_t avaliableChairs;
sem_t chair;
pthread_mutex_t mutex, mutex1;
pthread_cond_t condBarber;
pthread_cond_t condClient;
barberShop * BarberShop;
int clientNumber = 1;

void delay(int secs) { // função de daly
    time_t beg = time(NULL), end = beg + secs;
    do ; while (time(NULL) < end);
}

void * execClient(void * n) {
    pthread_mutex_lock(&mutex1);
    if (BarberShop -> queueSize == MAX_CHAIRS) {
        printf("The client %d tried to entry to BarberShop! The queue is full!\n\n", clientNumber ++);
        pthread_mutex_unlock(&mutex1);
        pthread_exit(NULL);
    }
    if (BarberShop -> barberStatus == 0) {
        printf("The barber wake up by client %d!\n\n", clientNumber);
        BarberShop -> barberStatus = 1;
        pthread_cond_signal(&condBarber);
    }
    int localClientNumber = clientNumber ++;
    BarberShop -> queueSize ++;
    pthread_mutex_unlock(&mutex1);
    sem_wait(&avaliableChairs);
    printf("the client %d joined the queue and and is waiting!\n\n", localClientNumber);
    sem_wait(&chair);
    BarberShop -> clientChair = localClientNumber;
    pthread_exit(NULL);
}

void * execBarber(void * n) {
    int previousClient = 0;
    while (1) {
        pthread_mutex_lock(&mutex);
        if (BarberShop -> queueSize == 0 && BarberShop -> barberStatus == 1) {
            printf("There are no customers in the queue! The barber is going to sleep!\n\n");
            BarberShop -> barberStatus = 0;
            pthread_mutex_unlock(&mutex);
            pthread_cond_wait(&condBarber, &mutex);
        }
        else if (BarberShop -> clientChair != previousClient) {
            sem_post(&avaliableChairs);
            printf("The barber will cut the client's %d hair\n\n", BarberShop -> clientChair);
            delay(rand() % 5);
            printf("The barber cut the client's %d hair\n\n", BarberShop -> clientChair);
            sem_post(&chair);
            previousClient = BarberShop -> clientChair;
            BarberShop -> queueSize --;
        }
        pthread_mutex_unlock(&mutex);
    }
}

barberShop * initBarberShop() {
    barberShop * BarberShopLocal = (barberShop *) malloc(sizeof(barberShop));
    BarberShopLocal -> barberStatus = 0;
    BarberShopLocal -> queueSize = 0;
    BarberShopLocal -> clientChair = 0;
    return BarberShopLocal;
}

int main() {
    sem_init(&avaliableChairs, 0, MAX_CHAIRS);
    sem_init(&chair, 0, 1);
    BarberShop = initBarberShop();
    pthread_t Barber;
    pthread_create(&Barber, NULL, execBarber, NULL);

    while(1) {
        pthread_t client;
        pthread_create(&client, NULL, execClient, NULL);
        delay(rand() % 3);
    }
    return 0;
}