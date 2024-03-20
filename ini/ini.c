/*******************************************************************************
 *  @file      ini.c
 *  @brief     Библиотека для работы с ini файлами
 *  @author    Young Sideways
 *  @date      1.03.2024
 *  @version   1.0
 *  @copyright © Young Sideways, 2024. All right reserved.
 ******************************************************************************/

#include "ini.h"

#pragma region --- INCLUDES ---

#include <stdlib.h>
#include "ini.parser.h"

#pragma endregion


#pragma region --- MACROS ---



#pragma endregion


#pragma region --- TYPEDEFS ---



#pragma endregion

#pragma region --- UTILS ---



#pragma endregion

#pragma region --- CONSTRUCTORS / DESTRUCTORS ---

INI* ini_create() {
    INI* ini = (INI*)malloc(sizeof(INI));
    if (ini) {
        ini->path     = NULL;
        ini->sections = NULL;
        ini->size     = 0U;
        ini->capacity = 0U;
    }
    return ini;
}

INI* ini_open(_IN const char* path) {
    if (!path)
        return NULL;
    if (!*path)
        return NULL;

    INI* ini = ini_create();
    if (ini) {
        ini->path = path;
        ini_tokenize(ini);
    }
    return ini;
}

#pragma endregion

#pragma region --- FUNCIONS ---

extern struct ini_parse_error {
    ini_parse_error_type type;
    int row;
} ini_parse_error;
extern const char const* const ini_parse_errors[];

ini_parse_error_type ini_get_parse_error() {
    return ini_parse_error.type;
}

const char* ini_parse_error_stringify() {
    char buffer[256];
    snprintf(buffer, 256, ini_parse_errors[ini_parse_error.type], ini_parse_error.row);
    return buffer;
}

#pragma endregion

#pragma region ---  ---



#pragma endregion