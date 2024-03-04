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

#include <string.h>
#include <ctype.h>

#pragma endregion

#pragma region --- MACROS ---

#define DEFAULT(var) DEFAULT_##var

#define DEFAULT_INI_COMMENT '#' // 
#define DEFAULT_INI_ESCAPE  '\\' // Backsalsh

#define DEFAULT_SECTION_NAME          "root"
#define DEFAULT_SECTION_OPEN_BRACKET  '['
#define DEFAULT_SECTION_CLOSE_BRACKET ']'

#define DEFAULT_PROPERTY_DELIMITER '=' // equals sign (=, ASCII 0x3D)

#define DEFAULT_STRING_QUOTE '\"' // double quotes and apostrophe

#define _bool_string_alias_true  "true"
#define _bool_string_alias_false "false"

#define DEFAULT_HT_INIT_SIZE 8U
#define HT_SIZE_GROWTH(size) ((size) << 1) // x2 factor

#pragma endregion

#pragma region --- TYPEDEFS ---

struct ini_value {
    ini_value_type type;
    union {
        bool   _vbool  ;
        int    _vint   ;
        double _vdouble;
        char*  _vstring;
    };
};

struct ini_property {
    ini_section_t* section; //!< parent object

    ini_property_t* next;   //!< next property, if it has name-colision

    ini_key_t key;          //!< property key
    ini_value_t value;      //!< propety value
};

struct ini_section {
    INI* file;
    const char* name;

    size_t size;
    size_t capacity;
    ini_property_t* properties;
};

struct ini {
    char* path;
    FILE* file;

    ini_section_t** sections;
    size_t section_count;
};

typedef size_t hash_t;

#pragma endregion

#pragma region --- UTILS ---

void _trim(char* str) {
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

ini_section_t* _find_section(const char* name, INI* file) {
    for (ini_section_t* begin = file->sections, *end = begin + file->section_count; begin < end; begin++)
        if (strcmp(name, begin->name) == 0)
            return begin;
    return NULL;
}

ini_section_t* _alloc_section(const char* name, INI* parent) {
    struct ini_section* section = malloc(sizeof(struct ini_section));
    if (!section)
        return NULL;
    section->properties = calloc(DEFAULT(HT_INIT_SIZE), sizeof(ini_property_t*));
    if (!section->properties)
        return NULL;
    section->file = parent;
    section->name = name;
    section->size = 0U;
    section->capacity = DEFAULT(HT_INIT_SIZE);

    return section;
}
ini_section_t* _add_section(const char* name, INI* parent) {
    ini_section_t* section = _find_section(name, parent);
    if (section)
        return section;
    ini_section_t* new_sections = realloc(parent->sections, (parent->section_count + 1) * sizeof(ini_section_t*));
    if (!new_sections)
        return NULL;
    parent->sections = new_sections;
    parent->sections[parent->section_count] = _alloc_section(name, parent);
    if (parent->sections[parent->section_count] == NULL)
        return NULL;
    parent->section_count++;
    return parent->sections[parent->section_count - 1];
}
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

INI* ini_open(_IN const char* path) {
    if (!path)
        return NULL;
    if (!*path)
        return NULL;

    INI* ini = malloc(sizeof(struct ini));
    if (!ini)
        goto _FAILURE;

    if (ini->file = fopen(path, "r+"))
        goto _PARSE;

    if (ini->file = fopen(path, "w+")) {
        ini->sections = NULL;
        ini->section_count = 0U;
        goto _COMPLETE;
    }
    
    goto _FAILURE;

_PARSE: {
    // generate "root" section

    char buffer[1024];
    while (fgets(buffer, 1024, ini->file)) {
        _trim(buffer); // clean all buffer whitespaces

    _COMPLETE_TOKEN:
    _INVALID_TOKEN:
        continue;
    }

    }
_COMPLETE:
    ini->path = path;
_FAILURE:
    return ini;
}

#pragma endregion

#pragma region --- FUNCIONS ---



#pragma endregion
