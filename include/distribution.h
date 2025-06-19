#pragma once

#include "array.h"

#define DISTR_PRESET_COUNT 5

/**
 * Enum to identify a randomization distribution preset.
 */
typedef enum distr_preset_t {
    DISTR_PRESET_NONE,                  // This represents the absence of a preset.
    DISTR_PRESET_UNIFORM,               // The uniform preset describes a distribution with only equal weights. e.g. 1,1,1,1,1,1 (one 6-sided uniform die)
    DISTR_PRESET_TWODICE,               // The twodice preset describes a distribution that arises when using two uniform and equal dice of equal side count and using the sum of the two diced values as result. e.g. 0,1,2,3,4,5,6,5,4,3,2,1 (12 weights, two 6-sided uniform dice)
    DISTR_PRESET_UPSTAIRS,              // The upstairs preset describes a distribution with the first weight being 1 and incrementing by 1 for each following weight. e.g. 1,2,3,4,5,6
    DISTR_PRESET_DOWNSTAIRS             // The downstairs preset describes a distribution with the last weight being 1 and incrementing by 1 for each previous weight. e.g. 6,5,4,3,2,1
} distr_preset_t;

/**
 * Struct to store a randomization distribution.
 * The probability of a single outcome is the weight Wi of that outcome divided by the sum of all weights.
 * Instead of setting the weights manually, a distribution can be built from a preset
 * by setting it's preset and building the distribution with the distr_build function.
 * 
 * - uniform: The weights are all set to 1 making every outcome equally probable. e.g. 1,1,1,1,1,1 (one 6-sided uniform die)
 * 
 * - twodice: The weights that would arise when using two uniform and equal dice and using the sum of the two diced values as overall outcome. e.g. 0,1,2,3,4,5,6,5,4,3,2,1 (12 weights, two 6-sided uniform dice)
 * 
 * - upstairs: The first weight is 1 and each weight is it's previous neighbored weight + 1. e.g. 1,2,3,4,5,6
 * 
 * - downstairs: The last weight is 1 and each weight is it's next neighbored weight + 1. e.g. 6,5,4,3,2,1
 */
typedef struct distribution_t {
    distr_preset_t preset;              // The preset that should be used to build the distribution weights
    array_t weights;                    // The weight of each possible outcome of the distribution (element type: size_t)
} distribution_t;

/**
 * Struct to store information about a distribution preset.
 */
typedef struct distr_preset_info_t {
    char* name;                         // The name of the preset
} distr_preset_info_t;

// Information about each distribution_t value
extern distr_preset_info_t distr_preset_infos[DISTR_PRESET_COUNT];

/**
 * Creates an empty distribution with preset DISTR_PRESET_NONE and an empty array of weights.
 * @return The created distribution.
 */
distribution_t distr_create_empty();

/**
 * Creates a distribution with the given preset and an empty array of weights.
 * The distribution can be built according to the set preset with the distr_built function.
 * @param preset The preset the created distribution should have.
 * @return The created distribution.
 */
distribution_t distr_create(distr_preset_t preset);

/**
 * Checks if the given distribution is empty meaning it has preset DISTR_PRESET_NONE and an empty array.
 * @param distr The distribution that should be checked.
 * @return true if the distribution is empty or no distribution was given, false otherwise.
 */
bool distr_isempty(const distribution_t* distr);

/**
 * Frees the given distribution setting it's preset to DISTR_PRESET_NONE and freeing it's array of weights.
 * @param distr The distribution that should be freed.
 */
void distr_free(distribution_t* distr);

/**
 * Converts the string into the corresponding distribution.
 * It must either be one of the following presets:
 * uniform, twodice
 * 
 * or it must describe distribution weights in the following format where Wn is the n-th weight:
 * <W1>,<W2>,<W3>,<W4>,...,<Wn>
 * 
 * Each weight must be a positive integer value. They must be separated by commas ','.
 * @param str The string that should be converted.
 * @param error The address the error code should be stored in. If not given the error code is not stored.
 * 
 * - 0 successfully parsed distribution.
 * 
 * - 1 no string given.
 * 
 * - 2 unable to duplicate string.
 * 
 * - 3 weight value out of range (overflow/underflow).
 * 
 * - 4 invalid characters in weight (not a number).
 * 
 * - 5 remaining characters after number in weight string.
 * 
 * @return The distribution represented by the string, an empty distribution if the string could not be converted.
 */
distribution_t strtodistr(const char* str, int* error);

/**
 * Validates and builds the given distribution according to the given die_sides.
 * If the distribution has preset all it's previous weights are removed and
 * 
 * - DISTR_PRESET_NONE: it's weight count should match the die sides,
 * if the distribution has less weights than die sides the remaining weights are added as value 0,
 * if the distribution has more weights than die sides error code 2 is returned.
 * 
 * - DISTR_PRESET_UNIFORM: die_sides many weights of value 1 are added creating a uniform distribution. 
 * 
 * - DISTR_PRESET_TWODICE: die_sides must be even and die_sides many weights are added with values that create the same
 * distribution as two uniform and equal dice would create when using the sum of the two diced values as result.
 * 
 * - DISTR_PRESET_UPSTAIRS: die_sides many weights with the first weight being 1 and incrementing each following weight by 1 are added. e.g. 1,2,3,4,5,6 for 6-sided die.
 * 
 * - DISTR_PRESET_DOWNSTAIRS: die_sides many weights with the first weight being 1 and incrementing each following weight by 1 are added. e.g. 1,2,3,4,5,6 for 6-sided die.
 * 
 * @return The error code.
 * 
 * - 0 successfully built distribution.
 * 
 * - 1 no distribution given.
 * 
 * - 2 unknown preset.
 * 
 * - 3 more weights than die sides.
 * 
 * - 4 unable to add weight to distribution.
 * 
 * - 5 invalid die sides. must be an even number (% 2 == 0).
 * 
 * - 6 invalid die sides. must not be 0.
 */
int distr_build(distribution_t* distr, size_t die_sides);

/**
 * Builds a uniform distribution (preset DSTR_PRESET_UNIFORM) for the given die sides.
 * @param die_sides The number of die sides to build the distribution for.
 * @param error The address the error code should be stored at if given.
 * 
 * - 0 successfully built uniform distribution.
 * 
 * - 1 unable to add weight to uniform distribution.
 * @return The distribution if it was built successfully, an empty distribution otherwise.
 */
distribution_t distr_preset_build_uniform(size_t die_sides, int* error);

/**
 * Builds a twodice distribution (preset DSTR_PRESET_TWODICE) for the given die sides.
 * The distribution is the one that arises when using two uniform and equal dice, each with (die_sides / 2) sides,
 * and using the sum of the two diced values as result.
 * 
 * Example 1: die_sides = 12 (two uniform 6 sided dice)
 * 
 *  + |  1  2  3  4  5  6 
 * ---+-------------------
 *  1 |  2  3  4  5  6  7 
 *  2 |  3  4  5  6  7  8 
 *  3 |  4  5  6  7  8  9 
 *  4 |  5  6  7  8  9 10 
 *  5 |  6  7  8  9 10 11 
 *  6 |  7  8  9 10 11 12 
 * 
 * -> distribution sides: 1,2,3,4,5,6,7,8,9,10,11,12
 * -> distribution weights: 0,1,2,3,4,5,6,5,4,3,2,1
 * -> distribution (side=weight): 1=0,2=1,3=2,4=3,5=4,6=5,7=6,8=5,9=4,10=3,11=2,12=1
 * 
 * Example 2: die_sides = 20 (two uniform 10 sided dice)
 * 
 *  + |  1  2  3  4  5  6  7  8  9 10 
 * ---+-------------------------------
 *  1 |  2  3  4  5  6  7  8  9 10 11 
 *  2 |  3  4  5  6  7  8  9 10 11 12 
 *  3 |  4  5  6  7  8  9 10 11 12 13 
 *  4 |  5  6  7  8  9 10 11 12 13 14 
 *  5 |  6  7  8  9 10 11 12 13 14 15 
 *  6 |  7  8  9 10 11 12 13 14 15 16 
 *  7 |  8  9 10 11 12 13 14 15 16 17
 *  8 |  9 10 11 12 13 14 15 16 17 18 
 *  9 | 10 11 12 13 14 15 16 17 18 19 
 * 10 | 11 12 13 14 15 16 17 18 19 20 
 * 
 * -> distribution sides: 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
 * -> distribution weights: 0,1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1
 * -> distribution (side=weight): 1=0,2=1,3=2,4=3,5=4,6=5,7=6,8=7,9=8,10=9,11=10,12=9,13=8,14=7,15=6,16=5,17=4,18=3,19=2,20=1
 * 
 * @param die_sides The number of die sides to build the distribution for. It must be an even number > 1.
 * @param error The address the error code should be stored at if given.
 * 
 * - 0 successfully built twodice distribution.
 * 
 * - 1 unable to add weight to twodice distribution.
 * 
 * - 2 invalid die sides. must be an even number (% 2 == 0).
 * 
 * - 3 invalid die sides. must not be 0.
 * @return The distribution if it was built successfully, an empty distribution otherwise.
 */
distribution_t distr_preset_build_twodice(size_t die_sides, int* error);

/**
 * Builds an upstairs distribution (preset DSTR_PRESET_UPSTAIRS) for the given die sides.
 * The first weight is 1 and each following weight is it's previous neighbored weight + 1.
 * e.g. 1,2,3,4,5,6 (for a 6-sided die)
 * @param die_sides The number of die sides to build the distribution for.
 * @param error The address the error code should be stored at if given.
 * 
 * - 0 successfully built upstairs distribution.
 * 
 * - 1 unable to add weight to upstairs distribution.
 * @return The distribution if it was built successfully, an empty distribution otherwise.
 */
distribution_t distr_preset_build_upstairs(size_t die_sides, int* error);

/**
 * Builds an downstairs distribution (preset DSTR_PRESET_DOWNSTAIRS) for the given die sides.
 * The last weight is 1 and each previous weight is it's next neighbored weight + 1.
 * e.g. 6,5,4,3,2,1 (for a 6-sided die)
 * @param die_sides The number of die sides to build the distribution for.
 * @param error The address the error code should be stored at if given.
 * 
 * - 0 successfully built downstairs distribution.
 * 
 * - 1 unable to add weight to downstairs distribution.
 * @return The distribution if it was built successfully, an empty distribution otherwise.
 */
distribution_t distr_preset_build_downstairs(size_t die_sides, int* error);
