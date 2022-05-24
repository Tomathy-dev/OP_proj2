/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#ifndef UNTITLED3_MESIGNAL_H
#define UNTITLED3_MESIGNAL_H

#include <synchronization.h>
#include <main.h>
#include <signal.h>

// Signals structure that contains signals information
struct signals {
    int max_ops; //número máximo de operações
    sem_t* mutex; // semaforo para exclusão mutua
    struct operation* op; //array com resultados de ops executadas
    int* terminate_flag;
};

struct signals signals_t;

/**
 * Writes the operations status to the console.
 */
void write_operations();

/**
 * Ignores a signal.
 */
void ignore_signal();

/**
 * Makes the action associated to the signal.
 */
void register_signal(__sighandler_t s_handler);

/**
 * Register an alarm with a given time.
 */
void register_alarm(int time, __sighandler_t s_handler);

/**
 * Change the terminate flag to one.
 */
void change_terminate_flag();

#endif
