/*******************************************************************************
 *  @file      ini.utils.h
 *  @brief     Single header ini utils lib
 *  @author    Young Sideways
 *  @date      5.03.2024
 *  @copyright © young.sideways@mail.ru, 2024. All right reserved.
 ******************************************************************************/

#ifndef _INI_UTILS_H_
#define _INI_UTILS_H_

#pragma once

#pragma region --- INCLUDES ---

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma endregion

#pragma region --- MACROS ---



#pragma endregion

#pragma region --- FUNCTIONS ---

char* skpled(char* str) {
    if (str) {
        char* ptr = str;
        while (isspace(*ptr)) ptr++;
        memmove(str, ptr, strlen(ptr) + 1);
    }
    return str;
}

char* skptai(char* str) {
    if (str) {
        char* ptr = str + strlen(str) - 1;
        while (isspace(ptr)) ptr--;
        ptr[1] = '\0';
    }
    return str;
}

void trim(char* str) {
    if (!str)
        return;

    size_t length = strlen(str);

    char* first = str;
    char* last = str + length - 1;

    for (; first <= last && isspace(first); first++);
    for (; first <= last && isspace(last); last--);

    size_t size = (intptr_t)(last - first) + 1LL;

    if (size == length)
        return;

    memmove(str, first, size);
    str[size] = '\0';
}

void ctrim(char* str, const char* charset) {
    if (!str || !charset)
        return;
    if (!*str || !*charset)
        return;

    char* ptr = str;
    for (; *str; str++)
        if (!strchr(charset, *str)) {
            if (str != ptr)
                *ptr = *str;
            ptr++;
        }
    *ptr = '\0';
}

char* str_lower(char* str) {
    if (str)
        for(; *str = (char)tolower(*str); str++);
    return str;
}

char* str_upper(char* str) {
    if (str)
        for (; *str = (char)toupper(*str); str++);
}

#pragma endregion

#endif // !_INI_UTILS_H_
