#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

adjmat_t adjmat_create(size_t vertex_count, size_t edge_count, const edge_t* edges) {
    adjmat_t adjmat = { vertex_count, 0 };
    if (vertex_count == 0)
        return adjmat;
    adjmat.edges = malloc(vertex_count * vertex_count * sizeof(int32_t));
    if (!adjmat.edges)
        return (adjmat_t){ 0, 0 };
    memset(adjmat.edges, 0, vertex_count * vertex_count * sizeof(int32_t));
    if (edge_count == 0 || !edges)
        return adjmat;
    for (size_t i = 0; i < edge_count; i++) {
        const edge_t* edge = &edges[i];
        adjmat.edges[edge->from * vertex_count + edge->to] = 1;
        adjmat.edges[edge->from + edge->to * vertex_count] = -1;
    }
    return adjmat;
}

void adjmat_free(adjmat_t* adjmat) {
    if (adjmat->edges)
        free(adjmat->edges);
    *adjmat = (adjmat_t){};
}

void adjmat_print(const adjmat_t* adjmat) {
    if (!adjmat || adjmat->vertex_count == 0 || !adjmat->edges)
        return;
    printf("\x1b[0m");
    printf("%5s","");
    for (size_t i = 0; i < adjmat->vertex_count; i++)
        printf("%4lu", i);
    printf("\n%4s\x1b(0l", "");
    for (size_t i = 0; i < adjmat->vertex_count; i++)
        printf("qqqq");
    printf("qk\x1b(B\n");
    for (size_t i = 0; i < adjmat->vertex_count * adjmat->vertex_count; i++) {
        int32_t edge = adjmat->edges[i];
        if (i % adjmat->vertex_count == 0)
            printf("%3lu \x1b(0x\x1b(B", i / adjmat->vertex_count);
        if (i % adjmat->vertex_count == i / adjmat->vertex_count)
            printf("\x1b[95m");
        else if (edge == 0)
            printf("\x1b[90m");
        else if (edge < 0)
            printf("\x1b[96m");
        else if (edge > 0)
            printf("\x1b[93m");
        printf("%4d", edge);
        printf("\x1b[0m");
        if (i % adjmat->vertex_count == adjmat->vertex_count - 1)
            printf(" \x1b(0x\x1b(B\n");
    }
    printf("%4s\x1b(0m", "");
    for (size_t i = 0; i < adjmat->vertex_count; i++)
        printf("qqqq");
    printf("qj\x1b(B\n");
}
