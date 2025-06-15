#pragma once

/**
 * Struct containing two strings.
 */
typedef struct twostrs_t {
    char* strs[2];
} twostrs_t;

/**
 * Splits the given string at the first occurrence of splitchar by replacing it with the null-terminator '\0'.
 * @param str The string that should be split.
 * @param splitchar The char where the string should be split.
 * @return The first and second part of the split string. If the string does not contain the specified splitchar
 * strs[0] is set to the given string and strs[1] is 0. If no string was given both strs[0] and strs[1] are 0.
 */
twostrs_t strsplitnext(char* str, char splitchar);

/**
 * Splits the given string at the first occurrence of splitchar without modifying the given string.
 * @param str The string that should be split.
 * @param splitchar The char where the string should be split.
 * @return The first and second part of the split string. If the string does not contain the specified splitchar
 * strs[0] is set to the given string and strs[1] is 0. If no string was given both strs[0] and strs[1] are 0.
 */
const twostrs_t strsplitnextconst(const char* str, char splitchar);

/**
 * Copies the given string into a newly allocated buffer. Ownership of the returned buffer is transferred to the caller.
 * @param str The string that should be copied.
 * @return The newly allocated buffer containing a copy of the given string, 0 if no string was given or no new buffer could be allocated.
 */
char* strduplicate(const char* str);
