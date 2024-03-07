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

#include "ini.utils.h"
#include "ini.parser.h"

#pragma endregion


#pragma region --- MACROS ---

#define DEFAULT(var) DEFAULT_##var

#define DEFAULT_INI_COMMENT '#' // (#, ASCII 0x23)
#define DEFAULT_INI_ESCAPE  '\\' // backslash (\, ASCII 0x5C)

#define DEFAULT_SECTION_NAME          "root" // root
#define DEFAULT_SECTION_OPEN_BRACKET  '['
#define DEFAULT_SECTION_CLOSE_BRACKET ']'

#define DEFAULT_PROPERTY_DELIMITER '=' // equals sign (=, ASCII 0x3D)

#define _bool_true_alias  "true"

#define DEFAULT_PARSER_BUFFER_SIZE 2048
#define DEFAULT_HT_INIT_SIZE 8U
#define HT_SIZE_GROWTH(size) ((size) << 1) // x2 factor
#define HT_MAX_LOAD_FACTOR 0.75f           // 75%

#pragma endregion


#pragma region --- TYPEDEFS ---


static const char default_section_name[] = DEFAULT(SECTION_NAME);

typedef size_t hash_t;

struct ini_value {
    ini_value_type type;
    union {
        int    _vint   ;
        double _vdouble;
        char*  _vstring;
    };
};

struct ini_property {
    ini_section_t* section; //!< parent object

    ini_property_t* next;   //!< next property, if it has name-colision

    char* key;              //!< property key
    ini_value_t value;      //!< property value
};

struct ini_section {
    INI* file;                   //!< parent object
    hash_t hash;                 //!< section name hash
    char* name;                  //!< section name

    size_t size;                 //!< current count of properties
    size_t capacity;             //!< current allocated blocks for properties
    ini_property_t* properties;  //!< properties table pointer
};

struct ini {
    char* path;
    FILE* file;

    ini_section_t* sections;
    size_t section_count;
};


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
        hash = 33U * hash ^ (uint8_t) * (value++);
    return hash;
}

#pragma endregion


#pragma region --- UTILS ---


ini_section_t* _find_section(const char* name, INI* file) {
    hash_t section_hash = DEFAULT(string_hash)(name, /*UNUSED*/0U);
    for (int i = 0; i < file->section_count; i++)
        if (file->sections[i].hash == section_hash)
            if (str(name, file->sections[i].name) == 0)
                return file->sections + i;
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


static bool _is_special(char c) {
    switch (c)
    {
    case DEFAULT(INI_COMMENT):
    case DEFAULT(INI_ESCAPE):
    case DEFAULT(SECTION_OPEN_BRACKET):
    case DEFAULT(SECTION_CLOSE_BRACKET):
    case DEFAULT(PROPERTY_DELIMITER):
        return true;
    default:
        return false;
    }
}

void _tokenize(const char* str) {
    /*

      EOS - end of sring
      EOB - end of buffer

      static const char section_alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789_"
      static const char key_alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"

      Валидные строки:
        1. [section]
        2. <key> = <value>
        3. <key> \ = \ <value T> \ <valueT> \ <#comment> \ <comment>

    TODO: - Оптимизация парсинга условными переходами с состоянием текущего контекста поиска токена

    1. Получить первое вхождение спец-символа:
        case '[' :
            1. Найти следующий спец-символ:
                case ']':
                    - trim
                    - только латиница верхнего и нижнего регистров, цифры и знаки '_' ? OK : ОШИБКА
                default: --- ОШИБКА --- - '[', '#', '\\', EOS, EOB
            2. Найти следующий спец-символ:
                case '#':
                    - TODO: парсинг значения
                    - пропустить коментарий до конца строки или конца буфера
                case EOS, EOB:
                    - парсинг значения
                case '\\'
                    1. перенос строки
                        - проверка на пустоту после символа
                    2. экранирование '[', ']', '#', '\\'
                default:  --- ОШИБКА --- - '[', '#', '\\', EOB
        case '=':
            1. Проверить ключ на валидность:
                - trim
                - только латиница верхнего и нижнего регистров, цифры и знаки '_' ? OK : ОШИБКА
            2. Найти следующий спец-символ:
                case '[', ']':
                    - --- ОШИБКА ---
                case '#' : пропустить 
                case '\\':
                    1. перенос строки
                        - проверка на пустоту после символа ? OK : ОШИБКА
                    2. экранирование '[', ']', '#', '\\'
        case '\\':
            1. перенос строки
                - проверка на пустоту после символа ? OK : ОШИБКА
        case ']':
            --- ОШИБКА ---
        
                    
    
    
    
    */
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

    char buffer[DEFAULT(PARSER_BUFFER_SIZE)];
    while (fgets(buffer, DEFAULT(PARSER_BUFFER_SIZE), ini->file)) {
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
