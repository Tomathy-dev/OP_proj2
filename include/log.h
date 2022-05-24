/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#ifndef UNTITLED3_LOG_H
#define UNTITLED3_LOG_H

#include "main.h"

/* Função que trata de criar um ficheiro log_filename.
* Funciona para todas as actions que não tenham argumentos (stop, help)
*/
void log_default (char* action, struct main_data* data);

/* Função que trata de criar um ficheiro log_filename.
* Funciona exclusivamente para a action "request"
*/
void log_request (int client_id, int rest_id, char* dish, struct main_data* data);

/* Função que trata de criar um ficheiro log_filename.
* Funciona exclusivamente para a action "status"
*/
void log_status (int id, struct main_data* data);

/*Função que trata de escrever o tempo em que uma dada ação foi requisitada.
* Formato: YYYY-MM-DD hh:mm:ss.milisegundos
*/
void write_time (FILE* fp);

#endif
