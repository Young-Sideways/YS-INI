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

#include <string.h>
#include <ctype.h>

/**
 *  @brief remove unnecessary leading and tailing whitespaces
 *  @param str - 
 */
void trim(char* str) {
    if (!str)
        return NULL;
    size_t size = strlen(str) - 1;
    char* begin = str;
    char* end = str + size;

    while (isspace(begin) && begin <= end) begin++;
    while (isspace(end) && begin <= end) end--;

    size_t new_size = end - begin;

    if (begin == end || size == new_size)
        return str;

    memmove(str, begin, new_size + 1);
    str[new_size + 1] = '\0';
}

#endif // !_INI_UTILS_H_
