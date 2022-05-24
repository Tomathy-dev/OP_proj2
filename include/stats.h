/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <main.h>
#include <stdio.h>

#ifndef UNTITLED3_STATS_H
#define UNTITLED3_STATS_H

/*
 * Writes the statistics into a file.
 */
void write_file_statistics(struct main_data* data, struct semaphores* sems);

/**
 * Writes the requests statistics into a file.
 */
void write_request_statistics(struct main_data* data, struct semaphores* sems);

/**
 * Writes the process' statistics.
 */
void write_process_statistics(struct main_data* data);

#endif
