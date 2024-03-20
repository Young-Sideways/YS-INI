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

#include "ini.types.h"
#include <stdbool.h>

#pragma endregion

#pragma region --- CONSTRUCTORS / DESTRUCTORS ---

INI* ini_create();
INI* ini_open(_IN const char* path);
void ini_destroy(_IN INI* ini);

#pragma endregion

#pragma region --- FUNCIONS ---

ini_value ini_get_value(INI* file, const char* key, _NULLABLE const char* section);

void* ini_to_struct(INI* file, _NULLABLE const char* format, _NULLABLE const char* section);

#pragma endregion

#pragma region --- PARSER ADAPTER ---

#ifdef _INI_PARSER_H_
ini_parse_error_type ini_get_parse_error();
const char* ini_parse_error_stringify();
#endif

#pragma endregion

#endif // !_INI_H_
