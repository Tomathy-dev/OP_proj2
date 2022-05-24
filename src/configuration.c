/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <configuration.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Reads the application arguments from a file.
 */
void read_config_file(char* filename, struct main_data* data){
    char path[255];
    sprintf(path, "../%s", filename);
    FILE* file = fopen(path, "r");
    if(file == NULL){
        perror("Error open the configuration file\n");
    } else {
        fscanf(file, "%d", &data->max_ops);
        fscanf(file, "%d", &data->buffers_size);
        fscanf(file, "%d", &data->n_restaurants);
        fscanf(file, "%d", &data->n_drivers);
        fscanf(file, "%d", &data->n_clients);
        fscanf(file, "%s", data->log_filename);
        fscanf(file, "%s", data->statistics_filename);
        fscanf(file, "%d", &data->alarm_time);
        data->current_ops = 0;
    }
    if(fclose(file) == -1){
        perror("Error closing file\n");
    }
    if(data->max_ops < 1 || data->buffers_size < 1 || data->n_restaurants < 1 || data->n_drivers < 1 || data->n_clients < 1
       || strcmp(data->log_filename, "") == 0 || strcmp(data->statistics_filename, "") == 0 || data->alarm_time < 0){
        printf("Configuration file with wrong format!\n");
        exit(-1);
    }
}
