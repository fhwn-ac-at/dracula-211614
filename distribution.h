#pragma once

/**
 * Enum to identify a randomization distribution.
 */
typedef enum distribution_t {
    DISTR_UNIFORM
} distribution_t;

/**
 * Sets the randomization distribution.
 * @param newdist The distribution that should be set.
 * @return The previous distribution;
 */
distribution_t distribution_set(distribution_t newdist);

/**
 * Gets the current randomization distribution.
 * @return The current distribution;
 */
distribution_t distribution_get();
