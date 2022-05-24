/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <metime.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * Updates the clock with the real time.
 */
void update_time(struct timespec* timespec){
    if(clock_gettime(CLOCK_REALTIME, timespec) == -1){
        perror("Error getting the clock time\n");
        exit(-1);
    }
}

/**
 * Parses timespec to the format AAAA-DD-MM HH:MM:SS.mmm
 */
void parse_timespec(struct timespec timespec, char* buffer, int bufferSize){
    struct tm tm_t;
    localtime_r(&timespec.tv_sec, &tm_t);
    strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S.", &tm_t);
    int milliseconds = (int) round((double)timespec.tv_nsec/1.0e6);
    sprintf(buffer, "%s%03d", buffer, milliseconds);
}