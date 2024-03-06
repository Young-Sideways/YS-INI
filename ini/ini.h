/*******************************************************************************
 *  @file      ini.h
 *  @brief     Библиотека для работы с ini файлами
 *  @author    Young Sideways
 *  @date      1.03.2024
 *  @version   1.0
 *  @copyright © Young Sideways, 2024. All right reserved.
 ******************************************************************************/

#ifndef _INI_H_
#define _INI_H_

#pragma once

#pragma region --- INCLUDES ---

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#pragma endregion

#pragma region --- MACROS ---

#define _IN
#define _OUT
#define _INOUT
#define _NULLABLE

#define UNUSED(var) ((void)var)

#pragma endregion

#pragma region --- TYPEDEFS ---

typedef struct ini_value ini_value_t;

typedef struct ini_property ini_property_t;
typedef struct ini_section ini_section_t;
typedef struct ini INI;

typedef enum {
    INI_NONE      = 0x0U,

    INI_INT       = 0x1U,
    INI_DOUBLE    = 0x2U,
    INI_STRING    = 0x3U
} ini_value_type;

#pragma endregion

#pragma region --- CONSTRUCTORS / DESTRUCTORS ---

INI* ini_open(_IN const char* path);
void ini_close(_IN INI* ini);

#pragma endregion

#pragma region --- FUNCIONS ---


ini_value_t ini_get_value(INI* file, const char* key, const char* section);

    bool ini_to_bool(_IN const ini_value_t value, _IN const bool     defval);
     int ini_to_int (_IN const ini_value_t value, _IN const int      defval);
unsigned ini_to_uint(_IN const ini_value_t value, _IN const unsigned defval);
   char* ini_to_str (_IN const ini_value_t value, _IN const char*    defval);

void* ini_to_sruct(INI* file, const char* format, ini_section_t* section); // WIP

#pragma endregion

#pragma region ---  ---



#pragma endregion

#endif // !_INI_H_
