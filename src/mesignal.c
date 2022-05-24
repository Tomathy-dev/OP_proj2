/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <mesignal.h>
#include <synchronization.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

/**
 * Writes the operations status to the console.
 */
void write_operations(){
    semaphore_mutex_lock(signals_t.mutex);
    for(int i = 0; i < signals_t.max_ops; i++){
        if(signals_t.op[i].status == 'C'){
            printf("request:%d status:%c start:%ld restaurant:%d rest_time:%ld driver:%d driver_time:%ld client:%d "
                   "client_end_time:%ld\n", i, signals_t.op[i].status, signals_t.op[i].start_time.tv_sec, signals_t.op[i].requested_rest,
                   signals_t.op[i].rest_time.tv_sec, signals_t.op[i].receiving_driver, signals_t.op[i].driver_time.tv_sec,
                   signals_t.op[i].receiving_client, signals_t.op[i].client_end_time.tv_sec);
        } else if(signals_t.op[i].status == 'I' || signals_t.op[i].status == 'R' || signals_t.op[i].status == 'D'){
            printf("request:%d status:%c\n", i, signals_t.op[i].status);
        } else {
            printf("request:%d status:%c\n", i, 'I');
        }
    }
    semaphore_mutex_unlock(signals_t.mutex);
}

/**
 * Ignores a signal.
 */
void ignore_signal(){
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("Error ignoring signal\n");
        exit(-1);
    }
}

/**
 * Makes the action associated to the signal.
 */
void register_signal(__sighandler_t s_handler){
    struct sigaction sa;
    sa.sa_handler = s_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("Error registering signal\n");
        exit(-1);
    }
}

/**
 * Register an alarm with a given time.
 */
void register_alarm(int time, __sighandler_t s_handler){
    struct sigaction sa;
    sa.sa_handler = s_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGALRM, &sa, NULL) == -1){
        perror("Error registing signal\n");
        exit(-1);
    }
    struct itimerval val;
    val.it_interval.tv_sec = time;
    val.it_interval.tv_usec = 0;
    val.it_value.tv_sec = time;
    val.it_value.tv_usec = 0;
    if(setitimer(ITIMER_REAL, &val, 0) == -1){
        perror("Error setting the alarm\n");
        exit(-1);
    }
}

/**
 * Change the terminate flag to one.
 */
void change_terminate_flag(){
    *signals_t.terminate_flag = 1;
}