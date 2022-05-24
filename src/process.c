/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <process.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <restaurant.h>
#include <driver.h>
#include <client.h>
#include <mesignal.h>

/* Função que inicia um novo processo restaurante através da função fork do SO. O novo
* processo irá executar a função execute_restaurant respetiva, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_restaurant(int restaurant_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    pid_t pid = fork();
    if(pid == -1){
        printf("Erro ao criar processo. launch_restaurant:process.c\n");
        exit(1);
    }
    if(pid == 0){
        // Child process
        ignore_signal(); // Ignoring signal for child process
        exit(execute_restaurant(restaurant_id, buffers, data, sems));
    } else {
        // Main process«
        return pid;
    }
}


/* Função que inicia um novo processo motorista através da função fork do SO. O novo
* processo irá executar a função execute_driver, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    pid_t pid = fork();
    if(pid == -1){
        printf("Erro ao criar processo. launch_restaurant:process.c\n");
        exit(-1);
    }
    if(pid == 0){
        // Child process
        ignore_signal(); // Ignoring signal for child process
        exit(execute_driver(driver_id, buffers, data, sems));
    } else {
        // Main process
        return pid;
    }
}


/* Função que inicia um novo processo cliente através da função fork do SO. O novo
* processo irá executar a função execute_client, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_client(int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    pid_t pid = fork();
    if(pid == -1){
        printf("Erro ao criar processo. launch_restaurant:process.c\n");
        exit(-1);
    }
    if(pid == 0){
        // Child process
        ignore_signal(); // Ignoring signal for child process
        exit(execute_client(client_id, buffers, data, sems));
    } else {
        // Main process
        return pid;
    }
}


/* Função que espera que um processo termine através da função waitpid.
* Devolve o retorno do processo, se este tiver terminado normalmente.
*/
int wait_process(int process_id){
    int status;
    int wait_pid = waitpid(process_id, &status, 0);

    if(wait_pid != process_id){
        return -1;
    }
    if(WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    printf("Erro ao terminar processo filho. wait_process:process.c");
    exit(-1);
}