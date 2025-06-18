#include "simulator.h"

#include "cvts.h"
#include "loadingscreen.h"

#include <stdio.h>

simulator_t simulator_create_empty() {
    return (simulator_t){ 
        .game = 0,
        .sals = array_create(0, sizeof(snakeorladder_t), 0),
        .solidxs = array_create(0, sizeof(optional_size_t), 0),
        .sims = array_create(0, sizeof(simulation_t), 0)
    };
}

simulator_t simulator_create(const game_t* game, size_t simcount) {
    if (!game || simcount == 0)
        return simulator_create_empty();

    simulator_t simulator = (simulator_t){
        .game = game,
        .sals = array_create(0, sizeof(snakeorladder_t), 0),
        .solidxs = array_create(game->adjmat.vertex_count, sizeof(optional_size_t), 0),
        .sims = array_create(simcount, sizeof(simulation_t), 0)
    };

    // store the game's snakes and ladders in the simulator's soldsts and solidxs arrays
    const adjmat_t* adjmat = &simulator.game->adjmat;
    for (size_t cell = 0; cell < adjmat->vertex_count; cell++) {
        // determine optional snake or ladder destination from current cell
        optional_size_t solidx = {};
        snakeorladder_t sol = {};
        for (size_t j = 0; !solidx.present && j < adjmat->vertex_count; j++) {
            if (adjmat->edges[cell * adjmat->vertex_count + j] > 0) {
                solidx = (optional_size_t){ true, simulator.sals.size };
                sol = (snakeorladder_t){ cell, j };
            }
        }
        // add snake or ladder index for current cell to solidxs array and if present add snake or ladder destination to soldsts array
        if (!array_add(&simulator.solidxs, &solidx) || (solidx.present && !array_add(&simulator.sals, &sol)))
            simulator_free(&simulator);
    }

    // initialize simulations
    for (size_t i = 0; i < simcount; i++)
        if (!array_add(&simulator.sims, &(simulation_t){ .simulator = &simulator }))
            simulator_free(&simulator);

    return simulator;
}

void simulator_free(simulator_t* simulator) {
    if (!simulator)
        return;
    array_free(&simulator->sals);
    array_free(&simulator->solidxs);
    array_free(&simulator->sims);
    *simulator = simulator_create_empty();
}

void simulate(const game_t* game, size_t simcount) {
    // create simulator and loading screen
    simulator_t simulator = simulator_create(game, simcount);
    #ifdef DEBUG
    simulator_print(&simulator);
    #endif
    loadingscreen_t loadscreen = loadingscreen_create();

    // start rendering loading screen
    loadingscreen_start(&loadscreen);

    // start simulations
    for (size_t i = 0; i < simulator.sims.size; i++) {
        simulation_t* sim = array_get(&simulator.sims, i);
        int simres = thrd_create(&sim->thread, (thrd_start_t)simulation_run, sim);
        switch (simres) {
            case thrd_success:
                break;
            case thrd_nomem:
                fprintf(stderr, "%swarning:%s unable to allocate memory for simulation thread %lu.\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT), i);
                break;
            case thrd_error:
                fprintf(stderr, "%swarning:%s unable to start simulation thread %lu.\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT), i);
                break;
            default:
                fprintf(stderr, "%swarning:%s unable to start simulation thread %lu for unexpected reason.\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT), i);
                break;
        }
    }

    // wait until all simulations finished
    for (size_t i = 0; i < simulator.sims.size; i++) {
        simulation_t* sim = array_get(&simulator.sims, i);
        int simres = 0;
        thrd_join(sim->thread, &simres);
        switch (simres) {
            case 0:
                break;
            default:
                fprintf(stderr, "%swarning:%s simulation thread %lu unexpectedly returned %d.\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT), i, simres);
                break;
        }
    }

    // stop rendering loading screen
    loadingscreen_stop(&loadscreen);

    // free simulator and loading screen
    loadingscreen_destroy(&loadscreen);
    simulator_free(&simulator);
}

int simulation_run(simulation_t* simulation) {
    if (!simulation)
        return 1;
    
    // TODO implement the simulation of a game
    
    return 0;
}

void simulator_print(const simulator_t* simulator) {
    if (!simulator) {
        printf("simulator = %s\n", (char*)0);
        return;
    }
    printf(
        "simulator = {\n"
        "  game = game_t @ %p,\n"
        "  sals = [%lu] {",
        simulator->game,
        simulator->sals.size
    );
    if (simulator->sals.size != 0) {
        printf("\n");
        for (size_t i = 0; i < simulator->sals.size; i++) {
            const snakeorladder_t* sol = array_getconst(&simulator->sals, i);
            printf("    [%lu] %lu-%lu%s\n", i, sol->src, sol->dst, i != simulator->sals.size - 1 ? "," : "");
        }
        printf("  ");
    }
    printf(
        "},\n"
        "  solidxs = [%lu] {",
        simulator->solidxs.size
    );
    if (simulator->solidxs.size != 0) {
        printf("\n");
        for (size_t i = 0; i < simulator->solidxs.size; i++) {
            const optional_size_t* idx = array_getconst(&simulator->solidxs, i);
            printf("    [%lu] %s%lu%s%s\n", i, idx->present ? FMT(FMTVAL_FG_DEFAULT) : FMT(FMTVAL_FG_BRIGHT_BLACK), idx->value, FMT(FMTVAL_FG_DEFAULT), i != simulator->solidxs.size - 1 ? "," : "");
        }
        printf("  ");
    }
    printf(
        "},\n"
        "  sims = [%lu] {",
        simulator->sims.size
    );
    if (simulator->sims.size != 0) {
        printf("\n");
        for (size_t i = 0; i < simulator->sims.size; i++) {
            const simulation_t* sim = array_getconst(&simulator->sims, i);
            printf(
                "    [%lu] {\n"
                "      thread  = %lu,\n"
                "      aborted = %s,\n"
                "      dices   = %lu\n"
                "      soluses = [%lu] {",
                i,
                (uint64_t)sim->thread,
                sim->aborted ? "true" : "false",
                sim->dices,
                sim->soluses.size
            );
            if (sim->soluses.size != 0) {
                printf("\n");
                for (size_t i = 0; i < sim->soluses.size; i++) {
                    const size_t* uses = array_getconst(&sim->soluses, i);
                    printf("        [%lu] %lu%s\n", i, *uses, i != simulator->solidxs.size - 1 ? "," : "");
                }
                printf("      ");
            }
            printf("},\n    },\n");
        }
        printf("  ");
    }
    printf("}\n}\n");
}
