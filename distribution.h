#pragma once

#define DISTRIBUTION_COUNT 2

/**
 * Enum to identify a randomization distribution.
 */
typedef enum distribution_t {
    DISTR_NONE,
    DISTR_UNIFORM
} distribution_t;

/**
 * Struct to store information about a distribution.
 */
typedef struct distribution_info_t {
    char shortname;
    char* longname;
} distribution_info_t;

// Information about each distribution_t value
extern distribution_info_t distribution_infos[DISTRIBUTION_COUNT];

/**
 * Converts the string into the corresponding distribution
 * @param str The that should be converted.
 * @return The distribution represented by the string, DISTR_NONE if the string represents no distribution.
 */
distribution_t strtodistr(const char* str);
