/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <main.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <utils.h>
#include <unistd.h>
#include <configuration.h>
#include <stats.h>
#include <metime.h>
#include <mesignal.h>
#include <log.h>

int main(int argc, char *argv[]) {
    //init data structures
    struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
    struct communication_buffers* buffers = create_dynamic_memory(sizeof(struct communication_buffers));
    buffers->main_rest = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->rest_driv = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->driv_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));

    //init semaphore data structure
    struct semaphores* sems = create_dynamic_memory(sizeof(struct semaphores));
    sems->main_rest = create_dynamic_memory(sizeof(struct prodcons));
    sems->rest_driv = create_dynamic_memory(sizeof(struct prodcons));
    sems->driv_cli = create_dynamic_memory(sizeof(struct prodcons));

    //execute main code
    main_args(argc, argv, data);
    create_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, buffers);
    create_semaphores(data, sems);
    launch_processes(buffers, data, sems);

    //init signals global variable structure
    signals_t.max_ops = data->max_ops;
    signals_t.mutex = sems->results_mutex;
    signals_t.op = data->results;
    signals_t.terminate_flag = data->terminate;
    // register sigint signal
    register_signal(change_terminate_flag);
    // register alarm signal
    register_alarm(data->alarm_time, write_operations);

    user_interaction(buffers, data, sems);

    stop_execution(data, buffers, sems);

    //release memory before terminating
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_rest);
    destroy_dynamic_memory(buffers->rest_driv);
    destroy_dynamic_memory(buffers->driv_cli);
    destroy_dynamic_memory(buffers);
    destroy_dynamic_memory(sems->main_rest);
    destroy_dynamic_memory(sems->rest_driv);
    destroy_dynamic_memory(sems->driv_cli);
    destroy_dynamic_memory(sems);
}

/* Função que lê os argumentos da aplicação, nomeadamente o número
* máximo de operações, o tamanho dos buffers de memória partilhada
* usados para comunicação, e o número de clientes, de motoristas e de
* restaurantes. Guarda esta informação nos campos apropriados da
* estrutura main_data.
*/
void main_args(int argc, char* argv[], struct main_data* data){
    if(argc == 2){
        read_config_file(argv[1], data);
    } else {
        printf("Uso: magnaeats config_file_name\n");
        printf("Exemplo: ./magnaeats config.txt\n");
        exit(-1);
    }
}

/* Função que reserva a memória dinâmica necessária para a execução
* do MAGNAEATS, nomeadamente para os arrays *_pids e *_stats da estrutura
* main_data. Para tal, pode ser usada a função create_dynamic_memory.
*/
void create_dynamic_memory_buffers(struct main_data* data){
    //IDs
    data->client_pids = create_dynamic_memory(sizeof(int) * data->n_clients);
    data->driver_pids = create_dynamic_memory(sizeof(int) * data->n_drivers);
    data->restaurant_pids = create_dynamic_memory(sizeof(int) * data->n_restaurants);
    //Stats
    data->client_stats = create_dynamic_memory(sizeof(int) * data->n_clients);
    data->driver_stats = create_dynamic_memory(sizeof(int) * data->n_drivers);
    data->restaurant_stats = create_dynamic_memory(sizeof(int) * data->n_restaurants);
}

/* Função que reserva a memória partilhada necessária para a execução do
* MAGNAEATS. É necessário reservar memória partilhada para todos os buffers da
* estrutura communication_buffers, incluindo os buffers em si e respetivos
* pointers, assim como para o array data->results e variável data->terminate.
* Para tal, pode ser usada a função create_shared_memory.
*/
void create_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers){

    //Buffers
    buffers->main_rest->buffer = create_shared_memory(STR_SHM_MAIN_REST_BUFFER, sizeof(struct operation) * data->buffers_size);
    buffers->rest_driv->buffer = create_shared_memory(STR_SHM_REST_DRIVER_BUFFER, sizeof(struct operation) * data->buffers_size);
    buffers->driv_cli->buffer = create_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, sizeof(struct operation) * data->buffers_size);

    //Pointers
    buffers->main_rest->ptrs = create_shared_memory(STR_SHM_MAIN_REST_PTR, sizeof(int) * data->buffers_size);
    buffers->rest_driv->ptrs = create_shared_memory(STR_SHM_REST_DRIVER_PTR, sizeof(struct pointers));
    buffers->driv_cli->ptrs = create_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, sizeof(int) * data->buffers_size);

    //Results&Terminate
    data->results = create_shared_memory(STR_SHM_RESULTS, sizeof(struct operation) * data->max_ops);
    data->terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
    
}

/* Função que inicia os processos dos restaurantes, motoristas e
* clientes. Para tal, pode usar as funções launch_*,
* guardando os pids resultantes nos arrays respetivos
* da estrutura data.
*/
void launch_processes(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    for(int i = 0; i < data->n_restaurants; i++){
        data->restaurant_pids[i] = launch_restaurant(i, buffers, data, sems);
    }
    for(int i = 0; i < data->n_drivers; i++){
        data->driver_pids[i] = launch_driver(i, buffers, data, sems);
    }
    for(int i = 0; i < data->n_clients; i++){
        data->client_pids[i] = launch_client(i, buffers, data, sems);
    }
}

/* Função que faz interação do utilizador, podendo receber 4 comandos:
* request - cria uma nova operação, através da função create_request
* status - verifica o estado de uma operação através da função read_status
* stop - termina o execução do MAGNAEATS através da função stop_execution
* help - imprime informação sobre os comandos disponiveis
*/
void user_interaction(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    // to save user input
    char action[7];
    // operation counter
    int op_counter = 0;

    show_menu();

    while(*data->terminate != 1){
        printf("Introduzir ação:\n");
        scanf("%s", (char*) &action);
        if(*data->terminate != 1){
            if(strstr(action, "request")){
                create_request(&op_counter, buffers, data, sems);
                data->current_ops += 1;
                usleep(31000);
            } else if(strstr(action, "status")){
                read_status(data, sems);
            } else if(strstr(action, "stop")){
                log_default(action, data);
                *data->terminate = 1;
            } else if(strstr(action, "help")){
                log_default(action, data);
                show_menu();
            } else {
                printf("Ação não reconhecida, insira 'help' para assistência.\n");
            }
        }
    }
}

/* Se o limite de operações ainda não tiver sido atingido, cria uma nova
* operação identificada pelo valor atual de op_counter e com os dados passados em
* argumento, escrevendo a mesma no buffer de memória partilhada entre main e restaurantes.
* Imprime o id da operação e incrementa o contador de operações op_counter.
*/
void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    if(*op_counter < data->max_ops){
        int client_id;
        int rest_id;
        char dish[20];
        scanf("%d %d %[^\n]", &client_id, &rest_id, dish);
        log_request(client_id, rest_id, dish, data);
        if(client_id < 0 || rest_id < 0 || client_id > data->max_ops || rest_id > data->max_ops){
            printf("id de operação é inválido!\n");
            return;
        }

        struct operation* op = malloc(sizeof (struct operation));
        op->id = *op_counter;
        op->requesting_client = client_id;
        op->requested_rest = rest_id;
        op->requested_dish = strdup(dish);
        op->status = 'I';
        struct timespec tm;
        tm.tv_nsec = 0;
        tm.tv_sec = 0;
        op->rest_time = tm;
        op->driver_time = tm;
        op->client_end_time = tm;
        update_time(&op->start_time);
        usleep(10000);
        // Writes the operation to the shared memory buffer
        produce_begin(sems->main_rest);
        write_main_rest_buffer(buffers->main_rest, data->buffers_size, op);
        produce_end(sems->main_rest);

        if(rest_id > data->n_restaurants - 1){
            data->results[*op_counter] = *op;
        }

        printf("O pedido #%d foi criado!\n", op->id);
        *op_counter += 1;
        free(op);
    } else {
        printf("O número máximo de pedidos foi alcançado!\n");
    }
}

/* Função que lê um id de operação do utilizador e verifica se a mesma
* é valida. Em caso afirmativo,
* imprime informação da mesma, nomeadamente o seu estado, o id do cliente
* que fez o pedido, o id do restaurante requisitado, o nome do prato pedido
* e os ids do restaurante, motorista, e cliente que a receberam e processaram.
*/
void read_status(struct main_data* data, struct semaphores* sems){
    int id;
    scanf("%d", &id);
    log_status(id, data);

    if(id < 0 || id > data->max_ops) {
        printf("id de pedido fornecido é inválido!\n");
        return;
    }

    semaphore_mutex_lock(sems->results_mutex);
    if(data->results[id].status == 'I'){
        printf("Pedido %d com o estado %c requisitado pelo cliente %d ao restaurante %d com o prato %s, ainda não"
               " foi recebido no restaurante!\n", id, data->results[id].status, data->results[id].requesting_client,
               data->results[id].requested_rest, data->results[id].requested_dish);
    } else if(data->results[id].status == 'R'){
        printf("Pedido %d com o estado %c requisitado pelo cliente %d ao restaurante %d com o prato %s, foi tratado"
               " pelo restaurante %d, mas ainda não foi encaminhado pelo motorista!\n", id, data->results[id].status,
               data->results[id].requesting_client, data->results[id].requested_rest, data->results[id].requested_dish,
               data->results[id].receiving_rest);
    } else if(data->results[id].status == 'D'){
        printf("Pedido %d com o estado %c requisitado pelo cliente %d ao restaurante %d com o prato %s, foi tratado"
               " pelo restaurante %d, encaminhado pelo motorista %d, mas ainda não foi recebido pelo cliente!\n",
               id, data->results[id].status, data->results[id].requesting_client, data->results[id].requested_rest,
               data->results[id].requested_dish, data->results[id].receiving_rest, data->results[id].receiving_driver);
    } else if(data->results[id].status == 'C'){
        printf("Pedido %d com o estado %c requisitado pelo cliente %d ao restaurante %d com o prato %s, foi tratado "
               "pelo restaurante %d, encaminhado pelo motorista %d e enviado ao cliente %d!\n", id, data->results[id].status,
               data->results[id].requesting_client, data->results[id].requested_rest, data->results[id].requested_dish,
               data->results[id].receiving_rest, data->results[id].receiving_driver, data->results[id].receiving_client);
    } else {
        printf("Pedido %d ainda não é válido!\n", id);
    }
    semaphore_mutex_unlock(sems->results_mutex);
}

/* Função que termina a execução do programa MAGNAEATS. Deve começar por
* afetar a flag data->terminate com o valor 1. De seguida, e por esta
* ordem, deve esperar que os processos filho terminem, deve escrever as
* estatisticas finais do programa, e por fim libertar
* as zonas de memória partilhada e dinâmica previamente
* reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
*/
void stop_execution(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems){
    wakeup_processes(data, sems);
    wait_processes(data);
    write_statistics(data);
    write_file_statistics(data, sems);
    destroy_semaphores(sems);
    destroy_memory_buffers(data, buffers);
}

/* Função que espera que todos os processos previamente iniciados terminem,
* incluindo restaurantes, motoristas e clientes. Para tal, pode usar a função
* wait_process do process.h.
*/
void wait_processes(struct main_data* data){
    for(int i = 0; i < data->n_restaurants; i++){
        data->restaurant_stats[i] = wait_process(data->restaurant_pids[i]);
    }
    for(int i = 0; i < data->n_drivers; i++){
        data->driver_stats[i] = wait_process(data->driver_pids[i]);
    }
    for(int i = 0; i < data->n_clients; i++){
        data->client_stats[i] = wait_process(data->client_pids[i]);
    }
}

/* Função que imprime as estatisticas finais do MAGNAEATS, nomeadamente quantas
* operações foram processadas por cada restaurante, motorista e cliente.
*/
void write_statistics(struct main_data* data){
    printf("Terminando o MAGNAEATS! Imprimindo estatísticas:\n");
    for(int i = 0; i < data->n_restaurants; i++){
        printf("Restaurante %d preparou %d pedidos!\n", i, data->restaurant_stats[i]);
    }
    for(int i = 0; i < data->n_drivers; i++){
        printf("Motorista %d entregou %d pedidos!\n", i, data->driver_stats[i]);
    }
    for(int i = 0; i < data->n_clients; i++){
        printf("Cliente %d recebeu %d pedidos!\n", i, data->client_stats[i]);
    }
}

/* Função que liberta todos os buffers de memória dinâmica e partilhada previamente
* reservados na estrutura data.
*/
void destroy_memory_buffers(struct main_data* data, struct communication_buffers* buffers){
    // destroy dynamic memory
    destroy_dynamic_memory(data->client_pids);
    destroy_dynamic_memory(data->driver_pids);
    destroy_dynamic_memory(data->restaurant_pids);
    destroy_dynamic_memory(data->client_stats);
    destroy_dynamic_memory(data->driver_stats);
    destroy_dynamic_memory(data->restaurant_stats);

    // destroy shared memory
    destroy_shared_memory(STR_SHM_MAIN_REST_BUFFER, buffers->main_rest->buffer, sizeof(struct operation) * data->buffers_size);
    destroy_shared_memory(STR_SHM_REST_DRIVER_BUFFER, buffers->rest_driv->buffer, sizeof(struct operation) * data->buffers_size);
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, buffers->driv_cli->buffer, sizeof(struct operation) * data->buffers_size);
    destroy_shared_memory(STR_SHM_MAIN_REST_PTR, buffers->main_rest->ptrs, sizeof(int) * data->buffers_size);
    destroy_shared_memory(STR_SHM_REST_DRIVER_PTR, buffers->rest_driv->ptrs, sizeof(struct pointers));
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, buffers->driv_cli->ptrs, sizeof(int) * data->buffers_size);
    destroy_shared_memory(STR_SHM_RESULTS, data->results, sizeof(struct operation) * data->max_ops);
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));
}

/* Função que inicializa os semáforos da estrutura semaphores. Semáforos
* *_full devem ser inicializados com valor 0, semáforos *_empty com valor
* igual ao tamanho dos buffers de memória partilhada, e os *_mutex com valor
* igual a 1. Para tal pode ser usada a função semaphore_create.
*/
void create_semaphores(struct main_data* data, struct semaphores* sems){
    // main-restaurant semaphores
    sems->main_rest->full  = semaphore_create(STR_SEM_MAIN_REST_FULL, 0);
    sems->main_rest->empty = semaphore_create(STR_SEM_MAIN_REST_EMPTY, data->buffers_size);
    sems->main_rest->mutex = semaphore_create(STR_SEM_MAIN_REST_MUTEX, 1);

    // restaurant-drivers semaphores
    sems->rest_driv->full  = semaphore_create(STR_SEM_REST_DRIV_FULL, 0);
    sems->rest_driv->empty = semaphore_create(STR_SEM_REST_DRIV_EMPTY, data->buffers_size);
    sems->rest_driv->mutex = semaphore_create(STR_SEM_REST_DRIV_MUTEX, 1);

    // drivers-client semaphores
    sems->driv_cli->full  = semaphore_create(STR_SEM_DRIV_CLI_FULL, 0);
    sems->driv_cli->empty = semaphore_create(STR_SEM_DRIV_CLI_EMPTY, data->buffers_size);
    sems->driv_cli->mutex = semaphore_create(STR_SEM_DRIV_CLI_MUTEX, 1);

    // results semaphores
    sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
}

/* Função que acorda todos os processos adormecidos em semáforos, para que
* estes percebam que foi dada ordem de terminação do programa. Para tal,
* pode ser usada a função produce_end sobre todos os conjuntos de semáforos
* onde possam estar processos adormecidos e um número de vezes igual ao
* máximo de processos que possam lá estar.
*/
void wakeup_processes(struct main_data* data, struct semaphores* sems){
    for(int i = 0; i < data->n_restaurants; i++){
        produce_end(sems->main_rest);
    }
    for(int i = 0; i < data->n_drivers; i++){
        produce_end(sems->rest_driv);
    }
    for(int i = 0; i < data->n_clients; i++){
        produce_end(sems->driv_cli);
    }
}

/* Função que liberta todos os semáforos da estrutura semaphores.
*/
void destroy_semaphores(struct semaphores* sems){
    // main-restaurant semaphores
    semaphore_destroy(STR_SEM_MAIN_REST_FULL, sems->main_rest->full);
    semaphore_destroy(STR_SEM_MAIN_REST_EMPTY, sems->main_rest->empty);
    semaphore_destroy(STR_SEM_MAIN_REST_MUTEX, sems->main_rest->mutex);

    // restaurant-drivers semaphores
    semaphore_destroy(STR_SEM_REST_DRIV_FULL, sems->rest_driv->full);
    semaphore_destroy(STR_SEM_REST_DRIV_EMPTY, sems->rest_driv->empty);
    semaphore_destroy(STR_SEM_REST_DRIV_MUTEX, sems->rest_driv->mutex);

    // drivers-client semaphores
    semaphore_destroy(STR_SEM_DRIV_CLI_FULL, sems->driv_cli->full);
    semaphore_destroy(STR_SEM_DRIV_CLI_EMPTY, sems->driv_cli->empty);
    semaphore_destroy(STR_SEM_DRIV_CLI_MUTEX, sems->driv_cli->mutex);

    // results semaphores
    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);
}
