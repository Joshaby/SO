# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# define NUM 5 // número de filósofos e garfos
# define THINKING 0 // status
# define HUNGRY 1
# define EATING 2

pthread_mutex_t mutex; // mutex para todos os filósofos
pthread_mutex_t mutex1; // mutex para todos os filósofos, na hora de verificar os vizinhos

typedef struct philosopher { // estrutura que represnta um filósofo
    int status; // status
    int position; // posição do filosofo na mesa
    struct philosopher *leftPhilosopher; // ponteiro para o filósofo esquerdo
    struct philosopher *rightPhilosopher; // ponteiro para o filósofo direito
} philosopher;

void delay(int secs) { // função de daly
  time_t beg = time(NULL), end = beg + secs;
  do {
      ;
  } while (time(NULL) < end);
}

philosopher * exec(philosopher * p) { // função que a thread irá executar! Função recebe um ponteiro para um philosopher
    while (1) {
        delay(rand() % 6);
        printf("The philosopher %d is thinking!\n\n", p -> position);
        getForks(p); // vai tentar pegar os garfos
        dropForks(p); // vai dropar os garfos, caso eles sejam pegos
    }
}

void getForks(philosopher * p) { // função para tentar pegar os garfos
    pthread_mutex_lock(&mutex); // obtem um bloqueio, para ocorrer atomicidade
    p -> status = HUNGRY; // define o estado HUNGRY
    printf("The philosopher %d is hungry\n\n", p -> position);
    delay(rand() % 6);
    verifyNeighbours(p); // verifica os vizinhos
    pthread_mutex_unlock(&mutex); // destrava o mutex
}

void dropForks(philosopher * p) { // função para dropar os garfos
    pthread_mutex_lock(&mutex); // obtem um bloqueio, para ocorrer atomicidade
    p -> status = THINKING; // define o estado THINKING
    verifyNeighbours(p -> leftPhilosopher); // vai verificar se os vizinhos podem comer
    verifyNeighbours(p -> rightPhilosopher);
    pthread_mutex_unlock(&mutex); // destrava o mutex
}

void verifyNeighbours(philosopher * p) { // função que verifica o status de cada vizinho
    pthread_mutex_lock(&mutex1); // obtem um bloqueio, para ocorrer atomicidade
    if (p -> status == HUNGRY && p -> leftPhilosopher -> status != EATING && p -> rightPhilosopher -> status != EATING) { // verifica os vizinhos
        p -> status == EATING;
        printf("The philosopher %d is eat\n\n", p -> position);
        delay(rand() % 6);
    }
    pthread_mutex_unlock(&mutex1); // destrava o mutex
}

int main() {

    pthread_attr_t *attr = NULL; // objeto de atributos da thread
    pthread_t * threads = (pthread_t *) malloc(NUM * sizeof(pthread_t)); // array de thread
    philosopher * philosophers = (philosopher *) malloc(NUM * sizeof(philosopher)); // array de philosopher

    for (int i = 0; i < NUM; i ++) { // setando atributos dos Filósofos
        philosophers[i].leftPhilosopher = NULL; // setando ponteiros dos vizinhos primeiramente como NULL
        philosophers[i].rightPhilosopher = NULL;
        philosophers[i].status = THINKING; // status atual
        philosophers[i].position = i; // posição
    }

    for (int i = 0; i < NUM; i ++) { // setando endereços dos vizinhos de um filósofo
        philosophers[i].leftPhilosopher = &philosophers[(i + NUM - 1) % NUM]; // vizinho esquerdo
        philosophers[i].rightPhilosopher = &philosophers[(i + 1) % NUM]; // vizinho direito
    }

    for (int i = 0; i < NUM; i ++) pthread_create(&threads[i], attr, exec, &philosophers[i]); // criando as threads
    for (int i = 0; i < NUM; i ++) pthread_join(threads[i], NULL); // fazendo join

    return 0;
}