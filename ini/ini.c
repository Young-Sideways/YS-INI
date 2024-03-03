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

#pragma endregion

#pragma region --- MACROS ---

#define DEFAULT(var) DEFAULT_##var

#define DEFAULT_INI_COMMENT "#;" // 
#define DEFAULT_INI_ESCAPE  '\\' // Backsalsh

#define DEFAULT_SECTION_NAME          "root"
#define DEFAULT_SECTION_OPEN_BRACKET  "["
#define DEFAULT_SECTION_CLOSE_BRACKET "]"

#define DEFAULT_PROPERTY_DELIMITER "=:" // equals sign (=, ASCII 0x3D) and a colon (:, ASCII 0x3A)

#define _bool_string_alias_true  "true"
#define _bool_string_alias_false "false"

#pragma endregion

#pragma region --- TYPEDEFS ---

struct ini_value {
    ini_value_type type;
    union {
        int      _int;
        double   _double;
        char*    _ascii;
        wchar_t* _unicode;
    };
};

struct ini_error {
    ini_error_type type;
    size_t row;
};

struct ini_property {
    ini_key_t key;
    ini_value_t value;
};

struct ini_section {
    const char* name;
    size_t table_size;
    size_t table_capacity;
    ini_property_t* entries;
};

struct ini {
    FILE* file;
    bool isOpen;

    ini_section_t* tables;
    size_t tables_count;
};

typedef size_t hash_t;

typedef hash_t(*hashfunc_t)(const void* data, size_t size);

#pragma endregion

#pragma region --- HASH ---

static hash_t DEFAULT(hash)(const void* data, size_t size) {
    hash_t hash = 5381U;
    for (uint8_t* value = (uint8_t*)data; size; size--)
        hash = 33U * hash ^ *(value++);
    return hash;
}
static hash_t DEFAULT(string_hash)(const void* data, size_t size) {
    UNUSED(size);

    hash_t hash = 5381U;
    for (char* value = (char*)data; *value; value++)
        hash = 33U * hash ^ (uint8_t)*(value++);
    return hash;
}
#pragma endregion

#pragma region --- CONSTRUCTORS / DESTRUCTORS ---



#pragma endregion

#pragma region --- FUNCIONS ---



#pragma endregion

#pragma region ---  ---



#pragma endregion

