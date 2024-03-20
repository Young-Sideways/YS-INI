/*******************************************************************************
 *  @file      ini.types.h
 *  @brief     Main ini types
 *  @author    Young Sideways
 *  @date      7.03.2024
 *  @copyright © Young Sideways, 2024. All right reserved.
 ******************************************************************************/

#ifndef _INI_TYPES_H_
#define _INI_TYPES_H_

#pragma once

#pragma region --- INCLUDES ---

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#pragma endregion

#pragma region --- MACROS ---

#define _IN
#define _OUT
#define _INOUT
#define _NULLABLE

#define UNUSED(var) ((void)var)

#pragma endregion

#pragma region --- TYPEDEFS ---

typedef uint32_t            ini_hash;

typedef enum ini_value_type ini_value_type;

typedef char*               ini_key;
typedef struct ini_value    ini_value;

typedef struct ini_property ini_property;
typedef struct ini_section  ini_section;
typedef struct ini          INI;

#pragma endregion

#pragma region --- ENUMS ---

enum ini_value_type {
    INI_NONE   = 0x0U,

    INI_INT    = 0x1U,
    INI_DOUBLE = 0x2U,
    INI_STRING = 0x3U
};

#pragma endregion

#pragma region --- STRUCTS ---

struct ini_value {
    ini_value_type type;
    union {
        int    vint;
        double vdouble;
        char*  vstring;
    };
};

struct ini_property {
    ini_section* section; //!< parent object

    ini_property* next;   //!< next property, if it has hash-collision

    ini_key key;          //!< property key
    ini_value value;      //!< property value
};

struct ini_section {
    INI* file;                 //!< parent object
    ini_hash hash;             //!< section name hash
    char* name;                //!< section name
    uint8_t depth;             //!< subsection depth (0 - section, > 0 - subsections)

    size_t size;               //!< current count of properties
    size_t capacity;           //!< current allocated blocks for properties
    ini_property** properties; //!< properties table pointer
};

struct ini {
    const char* path;       //!< path to the file

    ini_section** sections; //!< sections list
    size_t size;            //!< count of sections
    size_t capacity;        //!< count of sections
};

#pragma endregion

#pragma region --- FUNCIONS ---

ini_value ini_value_default(ini_value_type type) {
    switch (type)
    {
    case INI_INT:
        return (ini_value) { .type = type, .vint = 0 };
    case INI_DOUBLE:
        return (ini_value) { .type = type, .vdouble = 0.0 };
    case INI_STRING:
        return (ini_value) { .type = type, .vstring = NULL };
    default:
        return (ini_value) { .type = INI_NONE, .vdouble = 0.0 };
    }
}

bool ini_to_bool(_IN const ini_value value) {

    static const char _bool_string_alias[] = "true";

    switch (value.type)
    {
    case INI_INT:
        return !!value.vint;
    case INI_DOUBLE:
        return !!value.vdouble;
    case INI_STRING:
        if (!value.vstring)
            return false;
        return strcmp(value.vstring, _bool_string_alias) == 0;
    default:
        return false;
    }
}

int ini_to_int(_IN const ini_value value) {
    switch (value.type)
    {
    case INI_INT:
        return value.vint;
    case INI_DOUBLE:
        return (int)value.vdouble;
    case INI_STRING:
        if (!value.vstring)
            return 0;
        return (int)strtol(value.vstring, NULL, 0); //!< autodetect base;
    default:
        return 0;
    }
}

double ini_to_double(_IN const ini_value value) {
    switch (value.type)
    {
    case INI_INT:
        return (double)value.vint;
    case INI_DOUBLE:
        return value.vdouble;
    case INI_STRING:
        if (!value.vstring)
            return 0;
        return strtod(value.vstring, NULL);
    default:
        return 0.0;
    }
}

/**
 *  @brief  converts ini value to string
 *  @param  value - ini value
 *  @retval       - new null-terminating string with value
 *  @warning allocates memory block for new string
 */
char* ini_to_str(_IN const ini_value value) {
    char*  buffer = NULL;
    size_t length = 0;

    switch (value.type)
    {
    case INI_INT: {
        length = (size_t)snprintf(NULL, 0, "%d", value.vint); //!< find size of possible string
        buffer = (char*)malloc(length + 1);
        snprintf(buffer, length + 1, "%d", value.vint);
        break;
    }
    case INI_DOUBLE: {
        length = (size_t)snprintf(NULL, 0, "%f", value.vdouble); //!< find size of possible string
        buffer = (char*)malloc(length + 1);
        snprintf(buffer, length + 1, "%f", value.vdouble);
        break;
    }
    case INI_STRING:
        if (!value.vstring)
            break;
        length = strlen(value.vstring); //!< find size of possible string
        buffer = (char*)malloc(length + 1);
        memcpy(buffer, value.vstring, length + 1);
        break;
    default:
        break;
    }
    return buffer;
}

/**
 *  @brief  writes ini value to buffer
 *  @param  value  - ini value
 *  @param  buffer - memory block to put string in it
 *  @retval        - written buffer
 *  @warning The buffer must have enough memory to fit string, otherwise UB
 */
char* ini_to_buf(_IN const ini_value value, _INOUT char* buffer) {
    if (buffer)
        switch (value.type)
        {
        case INI_INT: {
            itoa(value.vint, buffer, 10);
            break;
        }
        case INI_DOUBLE: {
            sprintf(buffer,"%f", value.vdouble);
            break;
        }
        case INI_STRING:
            strcpy(buffer, value.vstring);
            break;
        default:
            buffer[0] = '\0';
            break;
        }
    return buffer;
}

#pragma endregion

#endif // !_INI_TYPES_H_
