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

# Example: Las Vegas Algorithm (05.06.2025)

We want to find a **random point** in the **unit circle** using a **Las Vegas** algorithm.

1. Generate a **random point** (x, y) with -1 < x < 1 and -1 < y < 1.
2. If the point is inside the unit circle, return it.
3. Continue from step 2.

function:

    point_t random_point_in_unit_circle() {
        point_t p;
        do {
            p = random_point();
        } while (is_point_in_unit_circle(p) == false);
        return p;
    }

On _average_, the algorithm runs 4/pi times before finding a point.
