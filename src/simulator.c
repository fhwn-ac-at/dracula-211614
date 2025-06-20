#include "simulator.h"

#include "assetmanager.h"
#include "cvts.h"
#include "loadingscreen.h"
#include "tsrand48.h"

#include <stdio.h>

simulation_t simulation_create_empty() {
    return (simulation_t){};
}

simulation_t simulation_create(simulator_t* simulator) {
    if (!simulator)
        return simulation_create_empty();
    simulation_t sim = {
        .simulator = simulator,
        .soluses = array_create(simulator->soldsts.size, sizeof(size_t), 0),
        .dices = array_create(0, sizeof(size_t), 0)
    };
    size_t inituseval = 0;
    for (size_t i = 0; i < sim.soluses.capacity; i++) {
        if (!array_add(&sim.soluses, &inituseval)) {
            simulation_free(&sim);
            return simulation_create_empty();
        }
    }
    return sim;
}

void simulation_free(simulation_t* simulation) {
    if (!simulation)
        return;
    array_free(&simulation->soluses, 0);
    array_free(&simulation->dices, 0);
    *simulation = simulation_create_empty();
}

simulator_t simulator_create_empty() {
    return (simulator_t){ 
        .game = 0,
        .dicelimit = 0,
        .soldsts = array_create(0, sizeof(size_t), 0),
        .solidxs = array_create(0, sizeof(optional_size_t), 0),
        .sims = array_create(0, sizeof(simulation_t), 0)
    };
}

simulator_t simulator_create(const game_t* game, size_t simcount, size_t dicelimit) {
    if (!game || simcount == 0)
        return simulator_create_empty();

    simulator_t simulator = (simulator_t){
        .game = game,
        .dicelimit = dicelimit,
        .soldsts = array_create(0, sizeof(size_t), 0),
        .solidxs = array_create(game->adjmat.vertex_count, sizeof(optional_size_t), 0),
        .sims = array_create(simcount, sizeof(simulation_t), 0)
    };

    // store the game's snakes and ladders in the simulator's soldsts and solidxs arrays
    const adjmat_t* adjmat = &simulator.game->adjmat;
    for (size_t cell = 0; cell < adjmat->vertex_count; cell++) {
        // determine optional snake or ladder destination from current cell
        optional_size_t solidx = {};
        size_t soldst = 0;
        for (size_t j = 0; !solidx.present && j < adjmat->vertex_count; j++) {
            if (adjmat->edges[cell * adjmat->vertex_count + j] > 0) {
                solidx = (optional_size_t){ true, simulator.soldsts.size };
                soldst = j;
            }
        }
        // add snake or ladder index for current cell to solidxs array and if present add snake or ladder destination to soldsts array
        if (!array_add(&simulator.solidxs, &solidx) || (solidx.present && !array_add(&simulator.soldsts, &soldst)))
            simulator_free(&simulator);
    }

    // initialize simulations
    for (size_t i = 0; i < simcount; i++) {
        simulation_t sim = simulation_create(&simulator);
        if (!array_add(&simulator.sims, &sim)) {
            simulation_free(&sim);
            simulator_free(&simulator);
        }
    }

    return simulator;
}

void simulator_free(simulator_t* simulator) {
    if (!simulator)
        return;
    array_free(&simulator->soldsts, 0);
    array_free(&simulator->solidxs, 0);
    array_free(&simulator->sims, (element_fn_t)simulation_free);
    *simulator = simulator_create_empty();
}

simulator_t simulate(const game_t* game, size_t simcount, size_t dicelimit) {
    // create simulator and loading screen
    simulator_t simulator = simulator_create(game, simcount, dicelimit);
    assetmanager_add(&simulator, (deallocator_fn_t)simulator_free);
    #ifdef DEBUG
    simulator_print(&simulator, 0, false);
    #endif
    loadingscreen_t loadscreen = loadingscreen_create("Simulating");

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

#ifdef DEBUG
    simulator_print(&simulator, 0, false);
#endif

    // free loading screen
    loadingscreen_destroy(&loadscreen);

    return simulator;
}

int simulation_run(simulation_t* simulation) {
    if (!simulation)
        return 1;

    // define helper variables
    const simulator_t* const simulator = simulation->simulator;
    const game_t* const game = simulator->game;
    const size_t lastcell = game->adjmat.vertex_count;

    // seed thread local rand48 random number generator for the die
    tsnewseed48();

    // start with player position outside the playing field (1 based index, i.e. first cell has index 1)
    simulation->playerpos = 0;
    while (simulation->playerpos != lastcell && simulation->dices.size < simulator->dicelimit) {
        // roll the die
        size_t side = dice(&game->die);
        array_add(&simulation->dices, &side);
        // end game if it should end
        if (simulation->playerpos + side == lastcell || (!game->exact_ending && simulation->playerpos + side > lastcell)) {
            simulation->playerpos = lastcell;
            break;
        } else if (simulation->playerpos + side > lastcell) {
            continue;
        }
        // move player
        simulation->playerpos += side;
        // check for presence of snake or ladder (0 based index, hence playerpos - 1)
        const optional_size_t* const solidx = array_getconst(&simulator->solidxs, simulation->playerpos - 1);
        if (solidx->present) {
            // use snake or ladder
            simulation->playerpos = *(const size_t*)array_getconst(&simulator->soldsts, solidx->value) + 1;
            // track usage of snake or ladder
            (*(size_t*)array_get(&simulation->soluses, solidx->value))++;
        }
    }
    // check if game was aborted due to reaching the dice limit before the game ended
    if (simulation->dices.size == simulator->dicelimit && simulation->playerpos != lastcell)
        simulation->aborted = true;

    return 0;
}

void simulator_print(const simulator_t* simulator, uint32_t indent, bool indentfirst) {
    if (!simulator) {
        printf("%*ssimulator = %s\n", indentfirst ? indent : 0, "", (char*)0);
        return;
    }
    printf(
        "%*ssimulator = {\n"
        "%*s  game      = game_t @ %p,\n"
        "%*s  dicelimit =  %lu,\n"
        "%*s  soldsts   = [%lu] {",
        indentfirst ? indent : 0, "",
        indent, "", simulator->game,
        indent, "", simulator->dicelimit,
        indent, "", simulator->soldsts.size
    );
    if (simulator->soldsts.size != 0) {
        printf("\n");
        for (size_t i = 0; i < simulator->soldsts.size; i++) {
            const size_t* soldst = array_getconst(&simulator->soldsts, i);
            printf("%*s    [%lu] %lu%s\n", indent, "", i, *soldst, i != simulator->soldsts.size - 1 ? "," : "");
        }
        printf("%*s  ", indent, "");
    }
    printf(
        "%*s},\n"
        "%*s  solidxs   = [%lu] {",
        indent, "",
        indent, "", simulator->solidxs.size
    );
    if (simulator->solidxs.size != 0) {
        printf("\n");
        for (size_t i = 0; i < simulator->solidxs.size; i++) {
            const optional_size_t* idx = array_getconst(&simulator->solidxs, i);
            printf("%*s    [%lu] %s%lu%s%s\n", indent, "", i, idx->present ? FMT(FMTVAL_FG_DEFAULT) : FMT(FMTVAL_FG_BRIGHT_BLACK), idx->value, FMT(FMTVAL_FG_DEFAULT), i != simulator->solidxs.size - 1 ? "," : "");
        }
        printf("%*s  ", indent, "");
    }
    printf(
        "%*s},\n"
        "%*s  sims      = [%lu] {",
        indent, "",
        indent, "", simulator->sims.size
    );
    if (simulator->sims.size != 0) {
        printf("\n");
        for (size_t i = 0; i < simulator->sims.size; i++) {
            printf("%*s    [%lu] ", indent, "", i);
            simulation_print(array_getconst(&simulator->sims, i), indent + 4, false);
        }
        printf("%*s  ", indent, "");
    }
    printf("}\n%*s}\n", indent, "");
}

void simulation_print(const simulation_t* simulation, uint32_t indent, bool indentfirst) {
    if (!simulation) {
        printf("%*ssimulation = %s\n", indentfirst ? indent : 0, "", (char*)0);
        return;
    }
    printf(
        "%*ssimulation = {\n"
        "%*s  simulator = simulator_t @ %p,\n"
        "%*s  thread    = %lu,\n"
        "%*s  aborted   = %s,\n"
        "%*s  playerpos = %lu,\n"
        "%*s  soluses   = [%lu] {",
        indentfirst ? indent : 0, "",
        indent, "", simulation->simulator,
        indent, "", simulation->thread,
        indent, "", simulation->aborted ? "true" : "false",
        indent, "", simulation->playerpos,
        indent, "", simulation->soluses.size
    );
    if (simulation->soluses.size != 0) {
        printf("\n");
        for (size_t i = 0; i < simulation->simulator->solidxs.size; i++) {
            const optional_size_t* idx = (optional_size_t*)array_getconst(&simulation->simulator->solidxs, i);
            if (idx->present) {
                snakeorladder_t sol = (snakeorladder_t){ i + 1, *(size_t*)array_getconst(&simulation->simulator->soldsts, idx->value) + 1 };
                const size_t* uses = array_getconst(&simulation->soluses, idx->value);
                printf("%*s    [%lu] %lu-%lu %lu%s\n", indent, "", idx->value, sol.src, sol.dst, *uses, idx->value != simulation->soluses.size - 1 ? "," : "");
            }
        }
        printf("%*s  ", indent, "");
    }
    printf(
        "},\n"
        "%*s  dices     = [%lu] {",
        indent, "", simulation->dices.size
    );
    if (simulation->dices.size != 0) {
        printf("\n");
        for (size_t i = 0; i < simulation->dices.size; i++) {
            const size_t* dice = (size_t*)array_getconst(&simulation->dices, i);
            printf("%*s    [%lu] %lu%s\n", indent, "", i, *dice, i != simulation->dices.size - 1 ? "," : "");
        }
        printf("%*s  ", indent, "");
    }
    printf("}\n%*s}\n", indent, "");
}
