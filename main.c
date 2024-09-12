#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_GARCONS 3
#define NUM_CLIENTES 10
#define NUM_MESAS 3

typedef struct{
    int id;
    int ocupado;
    pthread_mutex_t lock; // Mutex para sincronizar acesso à mesa
    pthread_cond_t cond;  // Condicional para sinalizar que a mesa está ocupada
} Mesa;

typedef struct {
    Mesa* mesa;
} Garcom;

Mesa mesas[NUM_MESAS];
Garcom garcons[NUM_GARCONS];

pthread_mutex_t mutexFila = PTHREAD_MUTEX_INITIALIZER; // Para gerenciar a fila de clientes
pthread_mutex_t mutexClienteCount = PTHREAD_MUTEX_INITIALIZER; // Para sincronizar contagem de clientes
pthread_cond_t todosClientesAtendidos = PTHREAD_COND_INITIALIZER;

int clientes_restantes = NUM_CLIENTES; // Variável global para contar clientes restantes
int terminou_atendimento = 0; // Sinaliza se o atendimento acabou

void* cliente(void* arg) {
    int id = *(int*)arg;
    free(arg);

    printf("Cliente %d quer uma mesa.\n", id);

    int mesa_atendida = -1;

    while (mesa_atendida == -1) {
        for (int i = 0; i < NUM_MESAS; i++) {
            pthread_mutex_lock(&mesas[i].lock); // Bloqueia a mesa atual para verificar

            if (!mesas[i].ocupado) {  // Se a mesa não está ocupada
                mesas[i].ocupado = 1; // O cliente ocupa a mesa
                mesa_atendida = i;    // Atribui a mesa para o cliente
                printf("Cliente %d sentou na mesa %d.\n", id, i+1);
                pthread_cond_signal(&mesas[i].cond); // Sinaliza o garçom para atender
                pthread_mutex_unlock(&mesas[i].lock); // Desbloqueia a mesa
                break;
            }

            pthread_mutex_unlock(&mesas[i].lock); // Desbloqueia se não for a mesa desejada
        }
        sleep(1); // Aguarda antes de tentar de novo se nenhuma mesa estava disponível
    }

    pthread_mutex_lock(&mutexClienteCount); // Bloqueia a variável de clientes restantes
    clientes_restantes--;

    // Se todos os clientes foram atendidos, sinaliza para os garçons
    if (clientes_restantes == 0) {
        terminou_atendimento = 1;
        pthread_cond_broadcast(&todosClientesAtendidos); // Notifica todos os garçons
    }

    pthread_mutex_unlock(&mutexClienteCount); // Desbloqueia a contagem de clientes

    return NULL;
}

void* garcom(void* arg) {
    Garcom* garcom = (Garcom*)arg;

    while (1) {
        // Bloqueia a mesa para aguardar clientes
        pthread_mutex_lock(&garcom->mesa->lock);

        while (!garcom->mesa->ocupado && !terminou_atendimento) {
            pthread_cond_wait(&garcom->mesa->cond, &garcom->mesa->lock); // Aguarda cliente
        }

        if (terminou_atendimento && !garcom->mesa->ocupado) {
            pthread_mutex_unlock(&garcom->mesa->lock); // Libera o lock e sai
            break;
        }

        printf("Garçom atendendo cliente na mesa %d.\n", (int)(garcom->mesa - mesas) + 1);

        // Simula o atendimento ao cliente
        sleep(2);

        // Atendimento finalizado
        printf("Garçom liberou a mesa %d.\n", (int)(garcom->mesa - mesas) + 1);
        garcom->mesa->ocupado = 0; // Libera a mesa para outros clientes

        pthread_mutex_unlock(&garcom->mesa->lock); // Desbloqueia a mesa
    }

    printf("Garçom terminou o atendimento na mesa %d e está saindo.\n", (int)(garcom->mesa - mesas) + 1);
    return NULL;
}

int main() {
    pthread_t clientes[NUM_CLIENTES];
    pthread_t threads_garcons[NUM_GARCONS];

    // Inicializa as mesas: define o ID, coloca como desocupada e inicializa mutexes e condições.
    for (int i = 0; i < NUM_MESAS; i++) {
        mesas[i].id = i + 1;
        mesas[i].ocupado = 0; // Marca a mesa como desocupada no início
        pthread_mutex_init(&mesas[i].lock, NULL); // Inicializa o mutex da mesa
        pthread_cond_init(&mesas[i].cond, NULL);  // Inicializa a condição de atendimento
    }

    // Inicializa os garçons e associa cada garçom a uma mesa específica.
    // Cria uma thread para cada garçom, que ficará responsável pela mesa atribuída.
    for (int i = 0; i < NUM_GARCONS; i++) {
        garcons[i].mesa = &mesas[i]; // Associa o garçom à mesa i
        pthread_create(&threads_garcons[i], NULL, garcom, &garcons[i]); // Cria uma thread para o garçom
    }

    // Cria threads para os clientes. Cada cliente é representado por uma thread separada.
    // O cliente tenta sentar-se em uma mesa disponível.
    for (int i = 0; i < NUM_CLIENTES; i++) {
        int* id_cliente = malloc(sizeof(int)); // Aloca memória para o ID do cliente
        *id_cliente = i + 1; // Atribui o ID do cliente (iniciando em 1)
        pthread_create(&clientes[i], NULL, cliente, id_cliente); // Cria uma thread para o cliente
    }

    // Aguarda o término de todas as threads de clientes.
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pthread_join(clientes[i], NULL); // Aguarda a conclusão da thread do cliente i
    }

    // Aguarda o término de todas as threads de garçons.
    for (int i = 0; i < NUM_GARCONS; i++) {
        pthread_join(threads_garcons[i], NULL); // Aguarda a conclusão da thread do garçom i
    }

    return 0;
}
