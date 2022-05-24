/* SO - Projeto1 - Grupo SO-052
 * Trabalho realizado pelos seguintes alunos:
 * fc51051 - Ivo Estrela
 * fc55180 - Ricardo Rocha
 * fc57528 - Tomás Salgueiro
*/

#include <memory-private.h>

void write_rnd_access_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation *op){
    int written = 0;
    int n = 0;
    while (n < buffer_size && written == 0) {
        if (buffer->ptrs[n] == 0) {
            buffer->buffer[n] = *(op);
            buffer->ptrs[n] = 1;
            written = 1;
        }
        n++;
    }
}

void write_circular_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
    if (buffer_size == 1 ||((buffer->ptrs->in + 1) % buffer_size) != buffer->ptrs->out) {
        buffer->buffer[buffer->ptrs->in] = *(op);
        (buffer->ptrs->in) = (buffer->ptrs->in + 1) % buffer_size;
    }
}

void read_circular_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
    if (buffer_size == 1 || (buffer->ptrs->in != buffer->ptrs->out)) {
        *(op) = buffer->buffer[buffer->ptrs->out];
        (buffer->ptrs->out) = (buffer->ptrs->out + 1) % buffer_size;
    } else {
        op->id = -1;
    }
}