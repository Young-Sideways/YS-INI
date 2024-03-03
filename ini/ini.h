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

#pragma endregion

#pragma region --- MACROS ---

#define _IN
#define _OUT
#define _INOUT
#define _NULLABLE

#define UNUSED(var) ((void)var)

#pragma endregion

#pragma region --- TYPEDEFS ---

typedef enum {
    ini_error_no_error  = 0x0,
    ini_error_undefined = -0x1
} ini_error_type;

typedef struct ini_error ini_error_t;

typedef char* ini_key_t;
typedef struct ini_value ini_value_t;

typedef struct ini_property ini_property_t;
typedef struct ini_section ini_section_t;
typedef struct ini INI;

typedef enum {
    INI_NONE      = 0x0U,

    INI_INT       = 0x1U,
    INI_DOUBLE    = 0x2U,
    INI_ASCII     = 0x3U,
    INI_UNICODE   = 0x4U,

    INI_UNDEFINED = -0x1
} ini_value_type;

#pragma endregion

#pragma region --- CONSTRUCTORS / DESTRUCTORS ---

INI* ini_open(_IN const char* file_path);
void ini_close(_IN _NULLABLE INI* ini);

#pragma endregion

#pragma region --- FUNCIONS ---



ini_error_t ini_error(_IN const INI* ini);

#pragma endregion

#pragma region ---  ---



#pragma endregion

#pragma region ---  ---



#pragma endregion

#endif // !_INI_H_
