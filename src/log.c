/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <log.h>

/* Função que trata de criar um ficheiro log_filename.
* Funciona para todas as actions que não tenham argumentos (stop, help)
*/
void log_default (char* action, struct main_data* data){
    char path[260];
    sprintf(path, "../%s", data->log_filename);
    FILE* fp;
    fp = fopen(path, "a");
    write_time(fp);
    fprintf(fp, "%s\n", action);
    fclose(fp);
}

/* Função que trata de criar um ficheiro log_filename.
* Funciona exclusivamente para a action "request"
*/
void log_request (int client_id, int rest_id, char* dish, struct main_data* data){
    char path[260];
    sprintf(path, "../%s", data->log_filename);
    FILE* fp;
    fp = fopen(path, "a");
    write_time(fp);
    fprintf(fp, "request %d %d %s\n", client_id, rest_id, dish);
    fclose(fp);
}

/* Função que trata de criar um ficheiro log_filename.
* Funciona exclusivamente para a action "status"
*/
void log_status (int id, struct main_data* data){
    char path[260];
    sprintf(path, "../%s", data->log_filename);
    FILE* fp;
    fp = fopen(path, "a");
    write_time(fp);
    fprintf(fp, "status %d\n", id);
    fclose(fp);
}

/*Função que trata de escrever o tempo em que uma dada ação foi requisitada.
* Formato: YYYY-MM-DD hh:mm:ss.milisegundos
*/
void write_time (FILE* fp){
    char buff[20];
    struct timeval tp;
    gettimeofday(&tp, 0);
    time_t currtime = tp.tv_sec;
    struct tm *t = localtime(&currtime);
    strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", t);
    fprintf(fp, "%s.%03ld ", buff, tp.tv_usec/1000);
}