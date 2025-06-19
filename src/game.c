#include "game.h"

#include "assetmanager.h"
#include "cli.h"
#include "cvts.h"

#include <stdlib.h>

game_t game_setup(cli_args_t* cli_args) {
    game_t game = {};
    if (!cli_args)
        return game;
    assetmanager_add(&game, (deallocator_fn_t)game_free);

    // set dimensions
    game.width = cli_args->width;
    game.height = cli_args->height;
    if (game.width < GAME_WIDTH_MIN || game.height < GAME_HEIGHT_MIN) {
        fprintf(stderr, "%serror:%s game has invalid dimensions %lux%lu. must be at least %lux%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), game.width, game.height, GAME_WIDTH_MIN, GAME_HEIGHT_MIN);
        exit(1);
    }
    size_t cellcount = game.width * game.height;

    // create die from distribution
    game.die = die_create(&cli_args->distribution);
    if (die_isempty(&game.die)) {
        fprintf(stderr, "%serror:%s unable to create die from distribution.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT));
        exit(1);
    }
    
    // set if game must have exact ending
    game.exact_ending = cli_args->exact_ending;

    // validate snakes and ladders
    array_t* sals = &cli_args->snakesandladders;
    for (size_t i = 0; i < sals->size; i++) {
        const snakeorladder_t* sol = array_getconst(sals, i);
        // check if sol starts or ends in a cell outside the playing field
        if (sol->src == 0 || sol->src > cellcount) {
            fprintf(stderr, "%serror:%s invalid %s %lu-%lu. starts in non-existant cell >%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), sol->src > sol->dst ? "snake" : "ladder", sol->src, sol->dst, cellcount - 1);
            exit(1);
        }
        if (sol->dst == 0 || sol->dst > cellcount) {
            fprintf(stderr, "%serror:%s invalid %s %lu-%lu. ends in a non-existant cell >%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), sol->src > sol->dst ? "snake" : "ladder", sol->src, sol->dst, cellcount - 1);
            exit(1);
        }
        // check if sol starts and ends in the same cell as itself
        if (sol->src == sol->dst) {
            fprintf(stderr, "%serror:%s invalid snake or ladder %lu-%lu. starts and ends in the same cell.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), sol->src, sol->dst);
            exit(1);
        }
        // check if sol starts or ends in the last cell of the playing field
        if (sol->src == cellcount) {
            fprintf(stderr, "%serror:%s invalid %s %lu-%lu. starts in the last cell %lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), sol->src > sol->dst ? "snake" : "ladder", sol->src, sol->dst, cellcount - 1);
            exit(1);
        }
        if (sol->dst == cellcount) {
            fprintf(stderr, "%serror:%s invalid %s %lu-%lu. ends in the last cell %lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT), sol->src > sol->dst ? "snake" : "ladder", sol->src, sol->dst, cellcount - 1);
            exit(1);
        }
        // check if sol overlaps with some other already existing sol (start/ends in the same cell as other sol)
        for (size_t j = 0; j < i; j++) {
            const snakeorladder_t* sol2 = array_getconst(sals, j);
            if (sol->src == sol2->src || sol->src == sol2->dst || sol->dst == sol2->src || sol->dst == sol2->dst) {
                fprintf(stderr, "%serror:%s invalid %s %lu-%lu. overlaps with %s %lu-%lu.\n", FMT(FMTVAL_FG_BRIGHT_RED), FMT(FMTVAL_FG_DEFAULT),
                    sol->src > sol->dst ? "snake" : "ladder", sol->src, sol->dst, sol2->src > sol2->dst ? "snake" : "ladder", sol2->src, sol2->dst);
                exit(1);
            }
        }
    }
    // convert snakes and ladders from 1 to 0 based indexing
    for (size_t i = 0; i < sals->size; i++) {
        snakeorladder_t* sol = array_get(sals, i);
        sol->src--;
        sol->dst--;
    }
    // create graph from snakes and ladders
    game.adjmat = adjmat_create(cellcount, sals->size, sals->data);
    
    return game;
}

void game_free(game_t* game) {
    if (!game)
        return;
    die_free(&game->die);
    adjmat_free(&game->adjmat);
    *game = (game_t){};
}

void game_print(const game_t* game) {
    if (!game || game->width == 0 || game->height == 0)
        return;
    // print reversed column numbers
    printf("%s", FMT(FMTVAL_DEFAULT));
    printf("%7s", "");
    for (size_t i = game->width; i > 0; i--)
        printf("%5lu", i);
    printf("\n");
    // print top outline
    printf("%6s\x1b(0l", "");
    for (size_t i = 0; i < game->width; i++)
        printf("qqqqq");
    printf("qk\x1b(B\n");
    for (size_t i = game->width * game->height; i > 0; i--) {
        // if the current row index is even print row in reverse order
        size_t idx = ((i-1) / game->width) % 2 == 0
            ? (((i-1) / game->width + 1) * game->width - 1) - ((i-1) % game->width)
            : (i-1);
        // find edge that starts or ends in current cell
        int hasedge = 0;
        int32_t edge = 0;
        for (size_t eidx = 0; eidx < game->adjmat.vertex_count; eidx++) {
            if (game->adjmat.edges[idx * game->adjmat.vertex_count + eidx] > 0) {
                hasedge = eidx < idx ? -1 : 1;
                edge = eidx + 1;
                break;
            }
        }
        // print index of first cell in row at the beginning of the line
        if ((i-1) % game->width == game->width - 1)
            printf("%5lu \x1b(0x\x1b(B", idx + 1);
        // print destination cell index for current edge
        if (hasedge == 0)
            printf("%s", FMT(FMTVAL_FG_BRIGHT_BLACK));
        else if (hasedge == 1)
            printf("%s", FMT(FMTVAL_FG_BRIGHT_YELLOW));
        else
            printf("%s", FMT(FMTVAL_FG_BRIGHT_CYAN));
        printf("%5d", edge);
        printf("%s", FMT(FMTVAL_DEFAULT));
        // print index of last cell in row at the end of the line
        if ((i-1) % game->width == 0)
            printf(" \x1b(0x\x1b(B %lu\n", idx + 1);
    }
    // print bottom outline
    printf("%6s\x1b(0m", "");
    for (size_t i = 0; i < game->width; i++)
        printf("qqqqq");
    printf("qj\x1b(B\n");
    // print column numbers
    printf("%s", FMT(FMTVAL_DEFAULT));
    printf("%7s", "");
    for (size_t i = 1; i <= game->width; i++)
        printf("%5lu", i);
    printf("\n");
}
