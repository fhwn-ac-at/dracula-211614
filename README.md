# Exercise

Implement a function that takes
- a number of vertices (numbered 0 to n) and
- a list of (positively) weighted nd directed edges
and

creates the **adjacency matrix** for the graph.

    struct Edge {
        int from;
        int to;
        int weight;
    };

The adjacency matrix should contain both predecessors and successors.
