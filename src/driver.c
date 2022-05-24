/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <driver.h>
#include <stdio.h>
#include <metime.h>
#include <unistd.h>

/* Função principal de um Motorista. Deve executar um ciclo infinito onde em
* cada iteração lê uma operação dos restaurantes e se a mesma tiver id
* diferente de -1 e se data->terminate ainda for igual a 0, processa-a e
* escreve a resposta para os clientes. Operações com id igual a -1 são
* ignoradas (op inválida) e se data->terminate for igual a 1 é porque foi
* dada ordem de terminação do programa, portanto deve-se fazer return do
* número de operações processadas. Para efetuar estes passos, pode usar os
* outros métodos auxiliares definidos em driver.h.
*/
int execute_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    int count = 0;
    for(;;){
        struct operation uber;
        driver_receive_operation(&uber, buffers, data, sems);
        if(*data->terminate == 1){
            return count;
        }
        if(uber.id != -1 && *data->terminate == 0){
            driver_process_operation(&uber, driver_id, data, &count, sems);
            driver_send_answer(&uber, buffers, data, sems);
        }
    }
}


/* Função que lê uma operação do buffer de memória partilhada entre restaurantes e motoristas.
* Antes de tentar ler a operação, deve verificar se data->terminate tem valor 1.
* Em caso afirmativo, retorna imediatamente da função.
*/
void driver_receive_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    consume_begin(sems->rest_driv);
    if (*data->terminate == 1) {
        return;
    }
    read_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
    consume_end(sems->rest_driv);
}


/* Função que processa uma operação, alterando o seu campo receiving_driver para o id
* passado como argumento, alterando o estado da mesma para 'D' (driver), e
* incrementando o contador de operações. Atualiza também a operação na estrutura data.
*/
void driver_process_operation(struct operation* op, int driver_id, struct main_data* data, int* counter, struct semaphores* sems){
    op->receiving_driver = driver_id;
    op->status = 'D';
    *counter +=1;
    data->results[op->id] = *op;
    update_time(&op->driver_time);
    usleep(10000);
    printf("Motorista recebeu pedido!\n");
}


/* Função que escreve uma operação no buffer de memória partilhada entre
* motoristas e clientes.
*/
void driver_send_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    produce_begin(sems->driv_cli);
    write_driver_client_buffer(buffers->driv_cli, data->buffers_size, op);
    produce_end(sems->driv_cli);
}

