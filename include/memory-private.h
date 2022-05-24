/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <memory.h>

#ifndef MAGNAEATS_MEMORY_PRIVATE_H
#define MAGNAEATS_MEMORY_PRIVATE_H

void write_rnd_access_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation *op);

void write_circular_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op);

void read_circular_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op);

#endif //MAGNAEATS_MEMORY_PRIVATE_H
