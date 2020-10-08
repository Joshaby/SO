# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# define NUM 5 // número de filósofos e garfos

typedef struct { // estrutura que represnta um filósofo
    int position; // posição do filosofo na mesa
    int **forksStatus; // lista que vai conter os endereços de memória dos garfos esquerdo e direito, que vai ser compartilhada com alguns filósofos
} philosopherstruct;

pthread_mutex_t mutex; // mutex geral
pthread_mutex_t forksMutex; // mutex para os garfos
int *forksStatusGlobal; // lista com os garfos

void delay(int secs) { // função de daly
  time_t beg = time(NULL), end = beg + secs;
  do {
    ;
  } while (time(NULL) < end);
}

philosopherstruct * exec(philosopherstruct * p) { // função que a thread irá executar! A função vai tentar pegar os garfos, caso sim, eles seram dropados! Função recebe um ponteiro para um philosopherstruct
    while (1) {
        printf("The philosopher %d is thinking\n\n", p -> position);
        delay(rand() % 5);
        pthread_mutex_lock(&mutex); // obtem a trava do mutex, para ocorrer atomicidade
        if (*p -> forksStatus[0] == 0 && *p -> forksStatus[1] == 0) { // verifica se os garfos estão disponíves
            *p -> forksStatus[0] = 1; // seta os garfos para 1, ou seja, vão ser usados
            *p -> forksStatus[1] = 1;
            pthread_mutex_unlock(&mutex); // destrava o mutex

            printf("The philosopher %d will eat the pasta! the philosopher took the forks %d and %d\n\n", p -> position, p -> position % NUM, (p -> position + 1) % NUM);
            delay(rand() % 3);
            printf("The philosopher %d ate the pasta\n\n", p -> position);

            pthread_mutex_lock(&forksMutex); // obtem a trava do mutex dos garfos
            *p -> forksStatus[0] = 0;// seta os garfos para 0, ou seja, não vão ser mais usados
            *p -> forksStatus[1] = 0;
            pthread_mutex_unlock(&forksMutex); // destratrava do mutex dos garfos
            printf("the philosopher %d dropped the forks\n\n", p -> position);
        }
        else { // caso os garfos não estejam disponíveis, vai ser printado que o filsofo não consegui comer, e mostrar qual filosofo esta com o garfo esquerdo ou direito
            if (*p -> forksStatus[0] != 0) printf("The philosopher %d couldn't eat! the fork was picked up by philosopher %d\n\n", p -> position, (p -> position + NUM + 1) % NUM);
            if (*p -> forksStatus[1] != 0) printf("The philosopher %d couldn't eat! the fork was picked up by philosopher %d\n\n", p -> position, (p -> position + 1) % NUM);
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main() {

    pthread_attr_t *attr = NULL; // objeto de atributos da thread
    philosopherstruct * philosophers = (philosopherstruct *) malloc(NUM * sizeof(philosopherstruct)); // array de philosopherstruct
    pthread_t philosophersThread[NUM]; // array de thread
    forksStatusGlobal = (int *) malloc(NUM * sizeof(int)); // array dos garfos

    for (int i = 0; i < NUM; i ++) forksStatusGlobal[i] = 0; // setando todos os garfos como não usavíéis

    for (int i = 0; i < NUM; i ++) { // setando atributos dos Filósofos
        philosophers[i].position = i; // posição do filósofo
        philosophers[i].forksStatus = (int **) malloc(2 * sizeof(int *)); // array das posições de memória dos garfos, um array de ponteiro para ponteiro
        philosophers[i].forksStatus[0] = &forksStatusGlobal[i % NUM]; // posição de memória do garfo esquerdo
        philosophers[i].forksStatus[1] = &forksStatusGlobal[(i + 1) % NUM]; // posição de memória do garfo direito
    }

    for (int i = 0; i < NUM; i ++) pthread_create(&philosophersThread[i], attr, exec, &philosophers[i]); // criando as threads
    for (int i = 0; i < NUM; i ++) pthread_join(philosophersThread[i], NULL); // fazendo join
}
