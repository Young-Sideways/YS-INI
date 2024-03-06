/*******************************************************************************
 *  @file      ini.utils.h
 *  @brief     single header ini utils lib
 *  @author    Young Sideways
 *  @date      5.03.2024
 *  @copyright © young.sideways@mail.ru, 2024. All right reserved.
 ******************************************************************************/

#ifndef _INI_UTILS_H_
#define _INI_UTILS_H_

#pragma once

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 *  @brief remove unecessary leading and tailing whitespaces
 *  @param str - string
 */
void trim(char* str) {
    if (!str)
        return;
    if (!*str)
        return;

    size_t size = strlen(str) - 1;
    char* begin = str;
    char* end = str + size;

    while (isspace(begin) && begin < end) begin++;
    while (isspace(end) && begin < end) end--;

    size_t new_size = end - begin;

    if (new_size < 1 || size == new_size)
        return;

    memmove(str, begin, new_size + 1);
    str[new_size + 1] = '\0';
}

/**
 *  @brief remove all unecessary whitespaces
 *  @param str - string pointer
 */
void trimall(char* str) {
    _CRT_UNUSED(str);
    // W.I.P

}

/**
 *  @brief remove all preset chars from string
 *  @param str     - string
 *  @param charset - preset chars
 */
void ctrim(char* const str, const char* const charset) {
    if (!str || !charset)
        return;
    if (!*str || !*charset)
        return;

    size_t size = strlen(str);

    char* trimmed = (char*)malloc(size + 1);
    if (trimmed) {
        size_t trimmed_size = 0;

        for (int i = 0; i < size; i++)
            if (!strchr(charset, str[i]))
                trimmed[trimmed_size++] = str[i];
        trimmed[trimmed_size++] = '\0';

        memcpy(str, trimmed, trimmed_size);
    }
}

void str_lower(char* str) {
    if (str)
        for (; *str; str++)
            if (isalpha(*str))
                *str = (char)tolower(*str);
}
#endif // !_INI_UTILS_H_
