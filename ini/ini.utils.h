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


char* _find_whitespace_sequince(char* str, size_t *size) {
    if (str)
        for (; *str; str++)
            if (isspace(str)) {
                char* wptr = str + 1;
                if (!isspace(wptr++))
                    continue;
                if (size) {
                    *size = 2;
                    while (*wptr && isspace(wptr))
                        (*size)++
                }
                return str;
            }
    return NULL;
}

/**
 *  @brief remove all unecessary whitespaces
 *  @param str - string pointer
 */
void trimall(char* str) {
    // _CRT_UNUSED(str);
    // W.I.P

    char* pos = strstr(str, '');
    while (pos) {
        size_t size = 1U;
        while (isspace(pos[size++]));
        // merge "... {some spaces} ..." -> "... { one space } ..."
    }
}

/**
 *  @brief remove all preset chars from string
 *  @param str     - string
 *  @param charset - preset chars
 */
void ctrim(char* str, const char* charset) {
    if (!str || !charset)
        return;
    if (!*str || !*charset)
        return;

    char* tptr = str;
    for (; *str; str++)
        if (str != tptr && strchr(charset, *str))
            *tptr++ = *str;
    tptr[1] = '\0';
}

void str_lower(char* str) {
    if (str)
        for (; *str; str++)
            *str = (char)tolower(*str);
}

#endif // !_INI_UTILS_H_
