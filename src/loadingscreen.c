#include "loadingscreen.h"

#include "cvts.h"

#include <stdio.h>

loadingscreen_t loadingscreen_create() {
    loadingscreen_t loadscreen = { .valid = true };
    int res = mtx_init(&loadscreen.mtx, mtx_timed);
    switch (res) {
        case thrd_success:
            break;
        case thrd_error:
            fprintf(stderr, "%swarning:%s unable to create mutex for loading screen.\n"
                            "         not rendering loading screen.\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
            return (loadingscreen_t){};
        default:
            fprintf(stderr, "%swarning:%s unable to create mutex for loading screen for unexpected reason.\n"
                            "         not rendering loading screen.\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
            return (loadingscreen_t){};
    }
    res = cnd_init(&loadscreen.cnd);
    switch (res) {
        case thrd_success:
            break;
        case thrd_nomem:
            fprintf(stderr, "%swarning:%s unable to allocate memory for condition for loading screen.\n"
                            "         not rendering loading screen.\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
            mtx_destroy(&loadscreen.mtx);
            return (loadingscreen_t){};
        case thrd_error:
            fprintf(stderr, "%swarning:%s unable to create condition for loading screen.\n"
                            "         not rendering loading screen.\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
            mtx_destroy(&loadscreen.mtx);
            return (loadingscreen_t){};
        default:
            fprintf(stderr, "%swarning:%s unable to create condition for loading screen for unexpected reason.\n"
                            "         not rendering loading screen.\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
            mtx_destroy(&loadscreen.mtx);
            return (loadingscreen_t){};
    }
    return loadscreen;
}

void loadingscreen_destroy(loadingscreen_t* loadscreen) {
    if (!loadscreen || !loadscreen->valid)
        return;
    cnd_destroy(&loadscreen->cnd);
    mtx_destroy(&loadscreen->mtx);
    *loadscreen = (loadingscreen_t){};
}

bool loadingscreen_start(loadingscreen_t* loadscreen) {
    if (!loadscreen || !loadscreen->valid || loadscreen->running)
        return false;
    int res = mtx_lock(&loadscreen->mtx);
    switch (res) {
        case thrd_success:
            break;
        case thrd_error:
            fprintf(stderr, "%swarning:%s unable to lock mutex for loading screen.\n"
                            "         not rendering loading screen\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
            break;
        default:
            fprintf(stderr, "%swarning:%s unable to lock mutex for loading screen for unexpected reason.\n"
                            "         not rendering loading screen\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
            break;
    }
    loadscreen->running = true;
    res = thrd_create(&loadscreen->thrd, (thrd_start_t)loadingscreen_run, loadscreen);
    switch (res) {
        case thrd_success:
            return true;
        case thrd_nomem:
            fprintf(stderr, "%swarning:%s unable to allocate memory for loading screen thread.\n"
                            "         not rendering loading screen\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
            break;
        case thrd_error:
            fprintf(stderr, "%swarning:%s unable to start loading screen thread.\n"
                            "         not rendering loading screen\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
            break;
        default:
            fprintf(stderr, "%swarning:%s unable to start loading screen thread for unexpected reason.\n"
                            "         not rendering loading screen\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
            break;
    }
    res = mtx_unlock(&loadscreen->mtx);
    switch (res) {
        case thrd_success:
            break;
        case thrd_error:
            fprintf(stderr, "%swarning:%s unable to unlock mutex for loading screen.\n"
                            "         not rendering loading screen\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
        default:
            fprintf(stderr, "%swarning:%s unable to unlock mutex for loading screen for unexpected reason.\n"
                            "         not rendering loading screen\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));
    }
    return false;
}

bool loadingscreen_stop(loadingscreen_t* loadscreen) {
    if (!loadscreen || !loadscreen->valid || !loadscreen->running)
        return false;
    cnd_signal(&loadscreen->cnd);
    thrd_join(loadscreen->thrd, 0);
    loadscreen->running = false;
    return true;
}

int loadingscreen_run(loadingscreen_t* loadscreen) {
    if (!loadscreen || !loadscreen->valid || !loadscreen->running)
        return 1;

    printf("  Simulating");

#define FRAMES_VARIANT 1

#if FRAMES_VARIANT == 0
    const char* frames[] = {
        "⠁", "⠃", "⠋", "⠏", "⠟", "⠿", "⠾", "⠼", "⠴", "⠰", "⠠", "⠰", "⠘",
        "⠈", "⠘", "⠙", "⠹", "⠻", "⠿", "⠷", "⠧", "⠦", "⠆", "⠄", "⠤",
        "⠠", "⠰", "⠴", "⠼", "⠾", "⠿", "⠟", "⠏", "⠋", "⠃", "⠁", "⠃", "⠆",
        "⠄", "⠆", "⠦", "⠧", "⠷", "⠿", "⠻", "⠹", "⠙", "⠘", "⠈", "⠉"
    };
#elif FRAMES_VARIANT == 1
    const char* frames[] = {
        "⠁", "⠃", "⠋", "⠏", "⠟", "⠿", "⠾", "⠼", "⠴", "⠰", "⠠", "⠠", "⠠", "⠠", "⠤", "⠤", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠉", "⠉", "⠈", "⠈", "⠈",
        "⠈", "⠘", "⠙", "⠹", "⠻", "⠿", "⠷", "⠧", "⠦", "⠆", "⠄", "⠄", "⠄", "⠄", "⠆", "⠆", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠰", "⠰", "⠠", "⠠", "⠠",
        "⠠", "⠰", "⠴", "⠼", "⠾", "⠿", "⠟", "⠏", "⠋", "⠃", "⠁", "⠁", "⠁", "⠁", "⠉", "⠉", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠤", "⠤", "⠄", "⠄", "⠄",
        "⠄", "⠆", "⠦", "⠧", "⠷", "⠿", "⠻", "⠹", "⠙", "⠘", "⠈", "⠈", "⠈", "⠈", "⠘", "⠘", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠃", "⠃", "⠁", "⠁", "⠁"
    };
#elif FRAMES_VARIANT == 2
    const char* frames[] = {
        "⠁", "⠃", "⠋", "⠚", "⠞", "⠖", "⠴", "⠤", "⠠", "⠠", "⠠", "⠤", "⠤", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠉", "⠉", "⠈", "⠈",
        "⠈", "⠉", "⠙", "⠓", "⠳", "⠲", "⠦", "⠆", "⠄", "⠄", "⠄", "⠆", "⠆", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠰", "⠰", "⠠", "⠠",
        "⠠", "⠰", "⠴", "⠖", "⠞", "⠚", "⠋", "⠉", "⠁", "⠁", "⠁", "⠉", "⠉", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠤", "⠤", "⠄", "⠄",
        "⠄", "⠤", "⠦", "⠲", "⠳", "⠓", "⠙", "⠘", "⠈", "⠈", "⠈", "⠘", "⠘", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠃", "⠃", "⠁", "⠁"
    };
#elif FRAMES_VARIANT == 3
    const char* frames[] = {
        "⠁", "⠉", "⠋", "⠚", "⠞", "⠖", "⠴", "⠰", "⠠", "⠠", "⠠", "⠤", "⠤", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠉", "⠉", "⠈", "⠈",
        "⠈", "⠘", "⠙", "⠓", "⠳", "⠲", "⠦", "⠤", "⠄", "⠄", "⠄", "⠆", "⠆", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠰", "⠰", "⠠", "⠠",
        "⠠", "⠤", "⠴", "⠖", "⠞", "⠚", "⠋", "⠃", "⠁", "⠁", "⠁", "⠉", "⠉", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠤", "⠤", "⠄", "⠄",
        "⠄", "⠆", "⠦", "⠲", "⠳", "⠓", "⠙", "⠉", "⠈", "⠈", "⠈", "⠘", "⠘", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠃", "⠃", "⠁", "⠁"
    };
#elif FRAMES_VARIANT == 4
    const char* frames[] = {
        "⠁", "⠉", "⠋", "⠚", "⠞", "⠖", "⠴", "⠰", "⠠", "⠠", "⠠", "⠤", "⠤", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠉", "⠉", "⠈", "⠈",
        "⠈", "⠘", "⠙", "⠓", "⠳", "⠲", "⠦", "⠤", "⠄", "⠄", "⠄", "⠆", "⠆", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠰", "⠰", "⠠", "⠠",
        "⠠", "⠤", "⠴", "⠖", "⠞", "⠚", "⠋", "⠃", "⠁", "⠁", "⠁", "⠉", "⠉", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠤", "⠤", "⠄", "⠄",
        "⠄", "⠆", "⠦", "⠲", "⠳", "⠓", "⠙", "⠉", "⠈", "⠈", "⠈", "⠘", "⠘", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠃", "⠃", "⠁", "⠁"
    };
#elif FRAMES_VARIANT == 5
    const char* frames[] = {
        "⠁", "⠃", "⠃", "⠋", "⠚", "⠞", "⠖", "⠴", "⠤", "⠤", "⠠", "⠠", "⠠", "⠤", "⠤", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠉", "⠉", "⠈", "⠈",
        "⠈", "⠉", "⠉", "⠙", "⠓", "⠳", "⠲", "⠦", "⠆", "⠆", "⠄", "⠄", "⠄", "⠆", "⠆", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠰", "⠰", "⠠", "⠠",
        "⠠", "⠰", "⠰", "⠴", "⠖", "⠞", "⠚", "⠋", "⠉", "⠉", "⠁", "⠁", "⠁", "⠉", "⠉", "⠙", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠤", "⠤", "⠄", "⠄",
        "⠄", "⠤", "⠤", "⠦", "⠲", "⠳", "⠓", "⠙", "⠘", "⠘", "⠈", "⠈", "⠈", "⠘", "⠘", "⠸", "⠴", "⠦", "⠇", "⠋", "⠙", "⠸", "⠴", "⠦", "⠇", "⠃", "⠃", "⠁", "⠁"
    };
#elif FRAMES_VARIANT == 6
    const char* frames[] = {
        "⠁", "⠃", "⠃", "⠋", "⠚", "⠞", "⠖", "⠴", "⠤", "⠤", "⠠", "⠠",
        "⠠", "⠤", "⠤", "⠶", "⠛", "⠉", "⠉", "⠈", "⠈",
        "⠈", "⠉", "⠉", "⠙", "⠓", "⠳", "⠲", "⠦", "⠆", "⠆", "⠄", "⠄",
        "⠄", "⠆", "⠇", "⠿", "⠿", "⠸", "⠰", "⠠", "⠠",
        "⠠", "⠰", "⠰", "⠴", "⠖", "⠞", "⠚", "⠋", "⠉", "⠉", "⠁", "⠁",
        "⠁", "⠉", "⠉", "⠛", "⠶", "⠤", "⠤", "⠄", "⠄",
        "⠄", "⠤", "⠤", "⠦", "⠲", "⠳", "⠓", "⠙", "⠘", "⠘", "⠈", "⠈",
        "⠈", "⠘", "⠸", "⠿", "⠿", "⠇", "⠃", "⠁", "⠁"
    };
#endif
    const int framecount = sizeof(frames) / sizeof(*frames);

    int res;
    struct timespec timeout;
    int frame = 0;
    cvts_cursor_hide();
    do {
        printf("\r%s%s%s", FMT(FMTVAL_FG_BRIGHT_BLUE), frames[frame = frame % framecount], FMT(FMTVAL_FG_DEFAULT));
        cvts_cursor_right(9999);
        fflush(stdout);
        frame++;
    
        timespec_get(&timeout, TIME_UTC);
        timeout.tv_nsec += 50000000l;
        if (timeout.tv_nsec >= 1000000000l) {
            timeout.tv_sec += 1l;
            timeout.tv_nsec %= 1000000000l;
        }
        res = cnd_timedwait(&loadscreen->cnd, &loadscreen->mtx, &timeout);
    } while (res == thrd_timedout);
    printf("\r%s⠿%s\n", FMT(FMTVAL_FG_BRIGHT_GREEN), FMT(FMTVAL_FG_DEFAULT));
    cvts_cursor_show();

    if (res != thrd_success)
        fprintf(stderr, "%swarning:%s loading screen failed.\n", FMT(FMTVAL_FG_YELLOW), FMT(FMTVAL_FG_DEFAULT));

    return 0;
}
