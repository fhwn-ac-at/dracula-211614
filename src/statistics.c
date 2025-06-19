#include "statistics.h"

#include "assetmanager.h"
#include "cvts.h"
#include "loadingscreen.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

stats_t stats_create() {
    return (stats_t){
        .dices = (valstats_t){ .min = ULONG_MAX },
        .shortestdices = array_create(0, sizeof(size_t), 0),
        .sals = array_create(0, sizeof(valstats_t), 0),
    };
}

void stats_free(stats_t* stats) {
    if (!stats)
        return;
    array_free(&stats->shortestdices, 0);
    array_free(&stats->sals, 0);
    *stats = stats_create();
}

stats_t stats_analyze(const simulator_t* simulator) {
    if (!simulator) {
        fprintf(stderr, "%serror:%s no simulator given statistical analysis.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
        exit(1);
    }
    if (simulator->sims.size == 0) {
        fprintf(stderr, "%serror:%s simulator has no simulations.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
        exit(1);
    }
    
    // create statistics
    stats_t stats = stats_create();

    // add stats to asset manager
    if (!assetmanager_add(&stats, (deallocator_fn_t)stats_free)) {
        fprintf(stderr, "%serror:%s unable to add stats to asset manager.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
        exit(1);
    }

    // prepare snakes and ladders stats array
    stats.sals = array_create(simulator->soldsts.size, sizeof(solstats_t), 0);
    for (size_t i = 0; i < simulator->solidxs.size; i++) {
        const optional_size_t* idx = (optional_size_t*)array_getconst(&simulator->solidxs, i);
        if (idx->present) {
            solstats_t solstats = (solstats_t){ .sol = { i, *(size_t*)array_getconst(&simulator->soldsts, idx->value) } };
            if (!array_add(&stats.sals, &solstats)) {
                fprintf(stderr, "%serror:%s unable to add stats about snake or ladder %lu to stats array.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), stats.sals.size);
                exit(1);
            }
        }
    }

    // number of run simulations
    stats.sims = simulator->sims.size;

    for (size_t i = 0; i < simulator->sims.size; i++) {
        const simulation_t* sim = array_getconst(&simulator->sims, i);

        // wins and losses
        if (sim->aborted)
            stats.losses++;
        else
            stats.wins++;
            
        // number of dices
        stats.dices.sum += sim->dices.size;
        if (stats.dices.min > sim->dices.size)
            stats.dices.min = sim->dices.size;
        if (stats.dices.max < sim->dices.size)
            stats.dices.max = sim->dices.size;
            
        // shortest dice sequence
        if (stats.shortestdices.size == 0 || stats.shortestdices.size > sim->dices.size)
            array_copy(&stats.shortestdices, &sim->dices);
            
        // snakes and ladders
        size_t simsalsuses = 0;
        size_t simsnakesuses = 0;
        size_t simladdersuses = 0;

        for (size_t i = 0; i < sim->soluses.size; i++) {
            // individual snake or ladder
            solstats_t* solstats = (solstats_t*)array_get(&stats.sals, i);
            size_t uses = *(const size_t*)array_getconst(&sim->soluses, i);
            solstats->uses.sum += uses;
            if (solstats->uses.min > uses)
                solstats->uses.min = uses;
            if (solstats->uses.max < uses)
                solstats->uses.max = uses;

            // all snakes and ladders in simulation
            simsalsuses += uses;
            if (solstats->sol.src > solstats->sol.dst)
                simsnakesuses += uses;
            else if (solstats->sol.src < solstats->sol.dst)
                simladdersuses += uses;
        }

        // all snakes and ladders in all simulations
        stats.salsuses.sum += simsalsuses;
        if (stats.salsuses.min > simsalsuses)
            stats.salsuses.min = simsalsuses;
        if (stats.salsuses.max < simsalsuses)
            stats.salsuses.max = simsalsuses;

        // all snakes in all simulations
        stats.snakesuses.sum += simsnakesuses;
        if (stats.snakesuses.min > simsnakesuses)
            stats.snakesuses.min = simsnakesuses;
        if (stats.snakesuses.max < simsnakesuses)
            stats.snakesuses.max = simsnakesuses;

        // all ladders in all simulations
        stats.laddersuses.sum += simladdersuses;
        if (stats.laddersuses.min > simladdersuses)
            stats.laddersuses.min = simladdersuses;
        if (stats.laddersuses.max < simladdersuses)
            stats.laddersuses.max = simladdersuses;
    }

    // average values
    if (stats.losses != 0)
        stats.winlossratio = (double)stats.wins / stats.losses;
    stats.dices.avg = (double)stats.dices.sum / stats.sims;
    for (size_t i = 0; i < stats.sals.size; i++) {
        solstats_t* solstats = (solstats_t*)array_get(&stats.sals, i);
        solstats->uses.avg = (double)solstats->uses.sum / stats.sims;
    }
    stats.salsuses.avg = (double)stats.salsuses.sum / stats.sims;
    stats.snakesuses.avg = (double)stats.snakesuses.sum / stats.sims;
    stats.laddersuses.avg = (double)stats.laddersuses.sum / stats.sims;

    return stats;
}

void stats_print(const stats_t* stats) {
    if (!stats)
        return;
    printf("\nStatistics\n\n");

    // TODO implement printing of statistics
    printf("// TODO // print statistics\n\n");
}
