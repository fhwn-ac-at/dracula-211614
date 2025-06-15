#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * Struct for an edge of a graph
 */
typedef struct edge_t {
    size_t from;
    size_t to;
} edge_t;

/**
 * Struct for the adjacency matrix of a graph
 */
typedef struct adjmat_t {
    size_t vertex_count;
    int32_t* edges;
} adjmat_t;

/**
 * Creates an adjacency matrix with the given amount of vertices and the given edges.
 * The validity of the provided edges is not checked.
 * @param vertex_count The amount of vertices in the graph.
 * @param edge_count The amount of given edges.
 * @param edges The edges of the graph.
 * @return The adjacency matrix of the graph. An empty one if vertex_count is 0 or it could not be created.
 */
adjmat_t adjmat_create(size_t vertex_count, size_t edge_count, const edge_t* edges);

/**
 * Deletes the given adjacency matrix freeing it's edges and setting it's size and edge pointer to 0.
 * @param adjmat The adjacency matrix that should be freed.
 */
void adjmat_delete(adjmat_t* adjmat);

/**
 * Prints the given adjacency matrix.
 * @param adjmat The adjacency matrix that should be printed.
 */
void adjmat_print(const adjmat_t* adjmat);
