/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#ifndef UNTITLED3_METIME_H
#define UNTITLED3_METIME_H

#include <time.h>

/**
 * Updates the clock with the real time.
 */
void update_time(struct timespec* timespec);

/**
 * Parses timespec to the format AAAA-DD-MM HH:MM:SS.mmm
 */
void parse_timespec(struct timespec timespec, char* buffer, int bufferSize);

#endif
