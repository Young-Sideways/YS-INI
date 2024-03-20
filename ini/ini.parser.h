/*******************************************************************************
 *  @file      ini.parser.h
 *  @brief     ini file syntax parser
 *  @author    Young Sideways
 *  @date      5.03.2024
 *  @copyright © young.sideways@mail.ru, 2024. All right reserved.
 ******************************************************************************/

#ifndef _INI_PARSER_H_
#define _INI_PARSER_H_

#pragma once

#pragma region --- INCLUDES ---

#include "ini.types.h"

#pragma endregion

#pragma region --- TYPEDEFS ---

typedef enum ini_parse_error_type ini_parse_error_type;

#pragma endregion

#pragma region --- ENUMS ---

enum ini_parse_error_type {
    EINI_NO,      // no errors
    EINI_NPAC,    // null pointer access
    EINI_MEMF,    // memory allocation fail
    EINI_OCF,     // open/create fail
    EINI_INVALTK, // invalid token
    EINI_LDTRSH,  // string leading trash
    EINI_TLTRSH,  // string tailing trash
    EINI_INSEC,   // invalid section name
    EINI_TOODP,   // subsection too deep
    EINI_UNBRCK   // unclosed section bracket
};

#pragma endregion

#pragma region --- FUNCTIONS ---

void ini_tokenize(INI* file);

#pragma endregion

#endif // !_INI_PARSER_H_
