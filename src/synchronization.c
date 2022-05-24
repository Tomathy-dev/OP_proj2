/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <synchronization.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

/* Função que cria um novo semáforo com nome name e valor inicial igual a
* value. Pode concatenar o resultado da função getuid() a name, para tornar
* o nome único para o processo.
*/
sem_t * semaphore_create(char* name, int value){
    char process_name[255];
    sprintf(process_name, "/%s_%d", name, getuid());
    sem_t* sem = sem_open(process_name, O_CREAT, 0xFFFFFFFF, value);
    if(sem == SEM_FAILED){
        perror("Semaphore creation failed.\n");
    }
    return sem;
}

/* Função que destroi o semáforo passado em argumento.
*/
void semaphore_destroy(char* name, sem_t* semaphore){
    char process_name[255];
    sprintf(process_name, "/%s_%d", name, getuid());
    if(sem_close(semaphore) == -1 ){
        perror("Can't destroy association between semaphore and process.\n");
    }
    if(sem_unlink(process_name) == -1){
        perror("Can't free semaphore resources.\n");
    }
}

/* Função que inicia o processo de produzir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_begin(struct prodcons* pc){
    if(sem_wait(pc->empty) == -1){
        perror("Error in produce_begin sem_wait(empty).\n");
    }
    if(sem_wait(pc->mutex) == -1){
        perror("Error in produce_begin sem_wait(mutex).\n");
    }
}

/* Função que termina o processo de produzir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_end(struct prodcons* pc){
    if(sem_post(pc->mutex) == -1){
        perror("Error in produce_end sem_post(mutex).\n");
    }
    if(sem_post(pc->full) == -1){
        perror("Error in produce_end sem_post(full).\n");
    }
}

/* Função que inicia o processo de consumir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_begin(struct prodcons* pc){
    if(sem_wait(pc->full) == -1){
        perror("Error in consume_begin sem_wait(full).\n");
    }
    if(sem_wait(pc->mutex) == -1){
        perror("Error in consuem_begin sem_wait(mutex).\n");
    }
}

/* Função que termina o processo de consumir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_end(struct prodcons* pc){
    if(sem_post(pc->mutex) == -1){
        perror("Error in consume_end sem_post(mutex).\n");
    }
    if(sem_post(pc->empty) == -1){
        perror("Error in consume_end sem_post(empty).\n");
    }
}

/* Função que faz wait a um semáforo.
*/
void semaphore_mutex_lock(sem_t* sem){
    if(sem_wait(sem) == -1){
        perror("Error in semaphore_mutex_lock sem_wait(sem)");
    }
}

/* Função que faz post a um semáforo.
*/
void semaphore_mutex_unlock(sem_t* sem){
    if(sem_post(sem) == -1){
        perror("Error in semaphore_mutex_unlock sem_post(sem)");
    }
}