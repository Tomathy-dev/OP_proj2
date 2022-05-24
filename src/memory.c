/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory-private.h>


/* Função que reserva uma zona de memória partilhada com tamanho indicado
* por size e nome name, preenche essa zona de memória com o valor 0, e
* retorna um apontador para a mesma. Pode concatenar o resultado da função
* getuid() a name, para tornar o nome único para o processo.
*/
void* create_shared_memory(char* name, int size){
    int* ptr;
    int ret;
    int uid = getuid();
    char sharedname[255];
    sprintf(sharedname,"/%s_%d", name, uid);
    int fd = shm_open(sharedname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(fd == -1){
        perror("Erro shm_open");
        exit(1);
    }

    ret = ftruncate(fd,size);
    if(ret == -1){
        perror("Erro ftruncate");
        exit(1);
    }

    ptr = mmap(NULL , size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        perror("Erro shm_mmap");
        exit(1);
    }

    memset(ptr,0,size);

    return ptr;
}


/* Função que reserva uma zona de memória dinâmica com tamanho indicado
* por size, preenche essa zona de memória com o valor 0, e retorna um
* apontador para a mesma.
*/
void* create_dynamic_memory(int size){
    int* ptr = calloc(1, size);
    if(ptr == NULL){
        exit(2);
    }
    return ptr;
}


/* Função que liberta uma zona de memória dinâmica previamente reservada.
*/
void destroy_shared_memory(char* name, void* ptr, int size){
    int ret = munmap(ptr, size);
    char sharedname[255];
    if(ret == -1){
        perror("Erro munmap");
        exit(3);
    }

    sprintf(sharedname, "/%s_%d", name, getuid());
    ret = shm_unlink(sharedname);
    if(ret == -1){
        perror("Erro smh_unlink");
        exit(3);
    }
}


/* Função que liberta uma zona de memória partilhada previamente reservada.
*/
void destroy_dynamic_memory(void* ptr){
    free(ptr);
}


/* Função que escreve uma operação no buffer de memória partilhada entre a Main
* e os restaurantes. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_main_rest_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    write_rnd_access_buffer(buffer, buffer_size, op);
}


/* Função que escreve uma operação no buffer de memória partilhada entre os restaurantes
* e os motoristas. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
    write_circular_buffer(buffer, buffer_size, op);
}


/* Função que escreve uma operação no buffer de memória partilhada entre os motoristas
* e os clientes. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_driver_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op){
    write_rnd_access_buffer(buffer, buffer_size, op);
}


/* Função que lê uma operação do buffer de memória partilhada entre a Main
* e os restaurantes, se houver alguma disponível para ler que seja direcionada ao restaurante especificado.
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_main_rest_buffer(struct rnd_access_buffer* buffer, int rest_id, int buffer_size, struct operation* op){
    int r = 0;
    int n = 0;
    while (n<buffer_size && r == 0) {
        if (buffer->ptrs[n] == 1) {
            (*op) = buffer->buffer[n];
            buffer->ptrs[n] = 0;
            r = 1;
        }
        n++;
    }
    if (r == 0) {
        op->id = -1;
    }
}


/* Função que lê uma operação do buffer de memória partilhada entre os restaurantes e os motoristas,
* se houver alguma disponível para ler (qualquer motorista pode ler qualquer operação).
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
    read_circular_buffer(buffer, buffer_size, op);
}


/* Função que lê uma operação do buffer de memória partilhada entre os motoristas e os clientes,
* se houver alguma disponível para ler dirijida ao cliente especificado. A leitura deve
* ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver
* nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_driver_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op){
    int r = 0;
    int n = 0;
    while (n<buffer_size && r == 0) {
        if (buffer->ptrs[n] == 1) {
            (*op) = buffer->buffer[n];
            buffer->ptrs[n] = 0;
            r = 1;
        }
        n++;
    }
    if (r == 0) {
        op->id = -1;
    }
}
