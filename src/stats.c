/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <stats.h>
#include <synchronization.h>
#include <metime.h>

/*
 * Writes the statistics into a file.
 */
void write_file_statistics(struct main_data* data, struct semaphores* sems){
    write_process_statistics(data);
    write_request_statistics(data, sems);
}

/**
 * Writes the requests statistics into a file.
 */
void write_request_statistics(struct main_data* data, struct semaphores* sems){
    char path[260];
    sprintf(path, "../%s", data->statistics_filename);
    FILE* file = fopen(path, "a");
    char file_line[350];
    char start_t[50], rest_t[50], driver_t[50], client_t[50];
    if(file == NULL){
        perror("Error opening statistics file\n");
    } else {
        if(fputs("\nRequest Statistics:\n", file) == -1){
            perror("Error writing in statistics file\n");
        }
        semaphore_mutex_lock(sems->results_mutex);
        for(int i = 0; i < data->current_ops; i++){
            if(data->results[i].status == 'C'){
                double total_t = ((double) data->results[i].client_end_time.tv_sec - (double) data->results[i].start_time.tv_sec) +
                                 ((double) data->results[i].client_end_time.tv_nsec - (double) data->results[i].start_time.tv_nsec) / 1000000000;
                parse_timespec(data->results[i].start_time, start_t, 50);
                parse_timespec(data->results[i].rest_time, rest_t, 50);
                parse_timespec(data->results[i].driver_time, driver_t, 50);
                parse_timespec(data->results[i].client_end_time, client_t, 50);
                sprintf(file_line, "Request: %d\nStatus: %c\nRestaurant id: %d\nDriver id: %d\nClient id: %d\nCreated: %s\n"
                                   "Restaurant time: %s\nDriver time: %s\nClient time (end): %s\nTotal time: %.3f\n\n",
                        data->results[i].id, data->results[i].status, data->results[i].requested_rest, data->results[i].receiving_driver,
                        data->results[i].requesting_client, start_t, rest_t, driver_t, client_t, total_t);
            } else {
                sprintf(file_line, "Request:%d\nStatus:%c\n\n", data->results[i].id, data->results[i].status);
            }
            if(fputs(file_line, file) == -1){
                perror("Error writing to statistics file\n");
            }
        }
        semaphore_mutex_unlock(sems->results_mutex);
        if(fclose(file) == -1){
            perror("Error closing statistics file\n");
        }
    }
}

/*
 * Writes the process' statistics into a file
 */
void write_process_statistics(struct main_data* data){
    char path[260];
    sprintf(path, "../%s", data->statistics_filename);
    FILE* file = fopen(path, "w");
    if(file == NULL){
        perror("Error opening statistics file\n");
    } else {
        if(fputs("Process Statistics:\n", file) == -1){
            perror("Error writing to statistics file");
        } else {
            char file_line[255];
            for(int i = 0; i < data->n_restaurants; i++){
                sprintf(file_line, "\t Restaurant %d prepared %d requests!\n", i, data->restaurant_stats[i]);
                if(fputs(file_line, file) == -1){
                    perror("Error writing to file\n");
                }
            }
            for(int i = 0; i < data->n_drivers; i++){
                sprintf(file_line, "\t Driver %d delivered %d requests!\n", i, data->driver_stats[i]);
                if(fputs(file_line, file) == -1){
                    perror("Error writing to file\n");
                }
            }
            for(int i = 0; i < data->n_clients; i++){
                sprintf(file_line, "\t Client %d received %d requests!\n", i, data->client_stats[i]);
                if(fputs(file_line, file) == -1){
                    perror("Error writing to file\n");
                }
            }
        }
        if(fclose(file) == -1){
            perror("Error closing statistics file\n");
        }
    }
}