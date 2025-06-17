#pragma once

#include <stdint.h>

/**
 * Parses the given string to a double.
 * @param str The string that should be parsed.
 * @param value The address the parsed double should be stored at.
 * If the parse failed the value remains unchanged. If no address was given the parsed double is not stored.
 * @return The error code.
 * - 0 parsed successfully
 * - 1 no string given
 * - 2 value out of range (overflow/underflow)
 * - 3 invalid characters (not a number)
 * - 4 remaining characters after number in string
 */
int strtodouble(const char* str, double* value);

/**
 * Parses the given string to a uint64_t.
 * @param str The string that should be parsed.
 * @param value The address the parsed uint64_t should be stored at.
 * If the parse failed the value remains unchanged. If no address was given the parsed uint64_t is not stored.
 * @return The error code.
 * - 0 parsed successfully
 * - 1 no string given
 * - 2 value out of range (overflow/underflow)
 * - 3 invalid characters (not a number)
 * - 4 remaining characters after number in string
 */
int strtouint64(const char* str, uint64_t* value);
