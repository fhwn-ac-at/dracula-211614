#pragma once

#include <stdbool.h>
#include <threads.h>

/**
 * Struct to store information for a loading screen.
 */
typedef struct loadingscreen_t {
    bool valid;                     // Indicates if the loading screen was successfully created
    bool running;                   // Indicates if the loading screen is currently running
    mtx_t mtx;                      // The timed mutex used for the condition in a loading screen
    cnd_t cnd;                      // The condition that should be signalled to stop the loading screen
    thrd_t thrd;                    // The thread that is run to render the loading screen
    const char* msg;                // The text that should be output next to the loading symbol
} loadingscreen_t;

/**
 * Creates a new loading screen with the given message initializing it's mutex and condition and setting valid to true on success.
 * If an error occurs an appropriate warning is output on stderr and
 * an empty loading screen with the member valid set to false is returned.
 * @param msg The message that should be printed next to the loading symbol. If no message is given no message is printed.
 * @return The created loading screen with valid set to true, an empty loading screen with valid set to false.
 */
loadingscreen_t loadingscreen_create(const char* msg);

/**
 * Destroys the given loading screen if it's member valid is true.
 * If no loading screen was given or valid is false no action is performed.
 * @param loadscreen The loading screen that should be destroyed.
 */
void loadingscreen_destroy(loadingscreen_t* loadscreen);

/**
 * Starts rendering the given loading screen on a separate thread if it is valid and not already running.
 * Rendering stops when the loadingscreen_stop function is called for the same loading screen.
 * If no loading screen was given, it is invalid or already running no action is performed and false is returned.
 * @param loadscreen The loading screen that should be started.
 * @return true if the loading screen was successfully started, false otherwise.
 */
bool loadingscreen_start(loadingscreen_t* loadscreen);

/**
 * Stops rendering the given loading screen by stopping the rendering thread if it is valid and currently running.
 * The calling thread blocks until the loading screen rendering thread stopped.
 * If no loading screen was given, it is invalid or currently not running no action is performed and false is returned.
 * @param loadscreen The loading screen that should be stopped.
 * @return true if the loading screen was successfully stopped, false otherwise.
 */
bool loadingscreen_stop(loadingscreen_t* loadscreen);

/**
 * Renders the given loading screen until it's condition is signalled if it is valid and not already running.
 * @param loadscreen The condition that must be signalled to stop rendering of the loading screen
 * @return Always 0.
 */
int loadingscreen_run(loadingscreen_t* loadscreen);
