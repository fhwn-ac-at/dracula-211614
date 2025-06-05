#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GENERATED_POINT_COUNT 30lu
#define APPROXIMATE_PI_ITERATIONS 1000000000lu

typedef struct point_t {
    double x;
    double y;
} point_t;

/**
 * Generates a point at a random position with -1 <= x <= 1 and -1 <= y <= 1.
 * @return The randomly generated point.
 */
point_t random_point() {
    return (point_t){
        .x = 1 - 2 * (double)rand() / (double)RAND_MAX,
        .y = 1 - 2 * (double)rand() / (double)RAND_MAX
    };
}

/**
 * Checks wether the given point lies within the unit circle
 * @param point The point that should be checked.
 * @return true if the point lies within the unit circle, false otherwise.
 */
bool is_point_in_unit_circle(point_t point) {
    return point.x * point.x + point.y * point.y <= 1.0;
}

/**
 * Generates a random point that lies within the unit circle.
 * @return The generated random point.
 */
point_t random_point_in_unit_circle() {
    point_t p;
    do {
        p = random_point();
    } while (is_point_in_unit_circle(p) == false);
    return p;
}

/**
 * Approximates pi with the monte carlo approach with the given number of iterations.
 * @param iterations The number of random points generated to approximate pi.
 * @return The approximated value of pi.
 */
double approximate_pi(size_t iterations) {
    if (iterations == 0)
        return 4.0;
    size_t inside = 0;
    for (size_t i = 0; i < iterations; i++) {
        point_t p = random_point();
        if (is_point_in_unit_circle(p))
            inside++;
    }
    return 4.0 * (double)inside / (double)iterations;
}

int main() {
    srand(time(0));

    printf("generate random points\n");
    for (size_t i = 0; i < GENERATED_POINT_COUNT; i++) {
        point_t p = random_point_in_unit_circle();
        printf("%2lu { x = % 1.6lf, y = % 1.6lf }\n", i, p.x, p.y);
    }

    printf("approximate pi\n");
    for (size_t i = 1; i < APPROXIMATE_PI_ITERATIONS; i *= 2) {
        double pi = approximate_pi(i);
        printf("%9lu pi = %1.6lf\n", i, pi);
    }
}
