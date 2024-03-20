/*******************************************************************************
 *  @file      ini.parser.c
 *  @brief     ini file syntax parser
 *  @author    Young Sideways
 *  @date      5.03.2024
 *  @copyright © young.sideways@mail.ru, 2024. All right reserved.
 ******************************************************************************/

#include "ini.parser.h"

 /*

   EOS - end of string
   EOB - end of buffer

   static const char section_alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789_"
   static const char key_alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_."

   Valid strings:
     1. [section]                   // section declaration
     2. [.subsection]               // subsection declaration
     3. [..***..subsection]         // n`th subsection declaration (max 16 depth)
     4. <key> =                     // key <key> with type "int" and value <0>(default int)
     5. <key> = <#comment>          // key <key> with type "int", value <0>(default int) and comment
     6. <key> = <value>             // key <key> with type "typeof(<value>)" and value <value>
     7. <key> = <value> <#comment>  // key <key> with type "typeof(<value>)", value <value>  and comment

 TODO: - Оптимизация парсинга условными переходами с состоянием текущего контекста поиска токена

 1. Получить первое вхождение спец-символа:
     case '[' :
         1. Найти следующий спец-символ:
             case ']':
                 - trim
                 - только латиница верхнего и нижнего регистров, цифры и знаки '_' ? OK : ОШИБКА

             default: --- ERROR --- - '[', '#', EOS, EOB
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
     case ']':
         --- ОШИБКА ---
 */

#pragma region --- INCLUDES ---

#include "ini.utils.h"

#pragma endregion

#pragma region --- MACROS ---

#define INI_COMMENT       '#'
#define INI_COMMENT_ALIAS ';'

#define SECTION_OPEN_BRACKET  '['
#define SECTION_CLOSE_BRACKET ']'
#define SECTION_NAME          "root" // root

#define PROPERTY_DELIMITER       '='
#define PROPERTY_DELIMITER_ALIAS ':'

#define INI_SPECIAL_CHARS "[]=:#;"

#define PARSER_BUFFER_SIZE 256

#define HT_INIT_SIZE 8U
#define HT_SIZE_GROWTH(size) ((size) << 1) // x2 factor
#define HT_MAX_LOAD_FACTOR 0.75f           // 75%

#pragma endregion

#pragma region --- HASH ---

static ini_hash default_hash(const void* data, size_t size) {
    ini_hash hash = 5381U;
    for (uint8_t* value = (uint8_t*)data; size; size--)
        hash = 33U * hash ^ *(value++);
    return hash;
}

static ini_hash default_str_hash(const void* data, size_t size) {
    UNUSED(size);

    ini_hash hash = 5381U;
    for (char* value = (char*)data; *value; value++)
        hash = 33U * hash ^ (uint8_t) * (value++);
    return hash;
}

#pragma endregion

#pragma region --- ERRORS ---



static const char const* const ini_parse_errors[] = {
    "no error",
    "error: null pointer access"
    "error: memory allocation fail"
    "error: cannot open/create ini file. incorrect path or the necessary privileges are missing",
    "error: bad ini syntax at %d row. invalid token",
    "error: bad section/subsection syntax at %d row. leading trash",
    "error: bad section/subsection syntax at %d row. tailing trash",
    "error: bad section/subsection syntax at %d row. invalid section name."
    "error: bad subsection syntax at %d row. too deep.",
    "error: bad section/subsection syntax at %d row. unclosed bracket."
};

static struct ini_parse_error {
    ini_parse_error_type type;
    int row;
} ini_parse_error = { .type = EINI_NO, .row = -1 };

#pragma endregion

#pragma region --- INTERNAL ---

/**
 *  @brief  allocates section block without binding to a parent ini file
 *  @param  name  - trimmed name
 *  @param  file  - ini file pointer
 *  @param  depth - valid section depth
 *  @return builded section block, or NULL if error
 */
ini_section * _section_alloc(const char* name, INI* file, uint8_t depth) {
    ini_section* section = malloc(sizeof(ini_section));
    if (!section)
        goto _FAIL_SECTION;

    size_t size = strlen(name);
    if (!(section->name = malloc(size + 1)))
        goto _FAIL_NAME;
    if (!(section->properties = calloc(HT_INIT_SIZE, sizeof(ini_property*))))
        goto _FAIL_PROPERTIES;

    memcpy(section->name, name, size + 1);
    section->file = file;
    section->hash = default_hash(name, size + 1);
    section->depth = depth;
    section->size = 0U;
    section->capacity = HT_INIT_SIZE;

    return section;

_FAIL_PROPERTIES:
    free(section->name);
_FAIL_NAME:
    free(section);
_FAIL_SECTION:
    ini_parse_error.type = EINI_MEMF;
    ini_parse_error.row = -1;
    return NULL;
}

void _section_merge(ini_section* lhs, ini_section* rhs) {

}

/**
 *  @brief binds section to a parent ini file
 *  @param section - valid section
 */
static void _section_integrate(ini_section* section) {
    if (section->file->sections[section->hash % section->file->capacity] != NULL) {

    }
}

ini_property* _property_alloc(const char* key, const ini_section* section) {
    ini_property* property = malloc(sizeof(ini_property));
    if (!property)
        goto _FAIL_PROPERTY;

    size_t size = strlen(key);
    if (!(property->key = malloc(size + 1)))
        goto _FAIL_NAME;

    property->section = section;
    property->next = NULL;
    memcpy(property->key, key, size + 1);
    property->value.type = INI_NONE;
    property->value.vdouble = 0.0;

    return property;

_FAIL_NAME:
    free(property);
_FAIL_PROPERTY:
    ini_parse_error.type = EINI_MEMF;
    ini_parse_error.row = -1;
    return NULL;
}

void _property_integrate(ini_property* property) {

}

/**
 *  @brief parses token to ini typed value
 *  @param property - valid property
 *  @param token    - valid trimmed token pointer
 */
void _property_parse_value_token(ini_property* property, const char* token) {
    size_t size = strlen(token);
    if (size == 0)
        goto _SET_DEFAULT;

    char* endptr = NULL;

    int int_value = strtol(token, &endptr, 0);
    if (*endptr == '\0') {
        property->value.type = INI_INT;
        property->value.vint = int_value;
        return;
    }
    double double_value = strtod(token, endptr);
    if (*endptr == '\0') {
        property->value.type = INI_DOUBLE;
        property->value.vint = double_value;
        return;
    }
    if (!(property->value.vstring = malloc(size + 1)))
        goto _FAIL_STRING;
    property->value.type = INI_STRING;
    memcpy(property->value.vstring, token, size + 1);

    return;

_SET_DEFAULT:
    property->value.type = INI_INT;
    property->value.vint = 0;
    return;
_FAIL_STRING:
    ini_parse_error.type = EINI_MEMF;
    ini_parse_error.row = -1;
    property->value.type = INI_NONE;
    property->value.vdouble = 0.0;
}

#pragma endregion

#pragma region --- FUNCTIONS ---

void ini_tokenize(INI* ini) {
    ini_parse_error.type = EINI_NO;
    ini_parse_error.row  = -1;

    FILE* file = fopen(ini->path, "r+");
    if (!file) {
        ini_parse_error.type = EINI_OCF;
        ini_parse_error.row = -1;
        return;
    }

    if (!(ini->sections = calloc(HT_INIT_SIZE, sizeof(ini_section*)))) {
        ini_parse_error.type = EINI_MEMF;
        ini_parse_error.row = -1;
        return;
    }
    ini->capacity = HT_INIT_SIZE;

    ini_section* current_section = NULL;

    // generate "root" section
    if (!(current_section = _section_alloc(SECTION_NAME, ini, 0)))
        return;



    // start parsing
    char buffer[PARSER_BUFFER_SIZE];
    size_t string_count = 0U;

    while (fgets(buffer, PARSER_BUFFER_SIZE - 1, file) != EOF) {
        char* first_special_char = strpbrk(buffer, INI_SPECIAL_CHARS);
        if (!first_special_char) {
            trim(buffer);
            if (strlen(buffer) != 0) {
                ini_parse_error.type = EINI_INVALTK;
                ini_parse_error.row  = string_count;
                break;
            }
            else
                continue;
        }
        switch (*first_special_char)
        {
        case INI_COMMENT:
        case INI_COMMENT_ALIAS:
            *first_special_char = '\0';
            trim(buffer);
            if (strlen(buffer) != 0) {
                ini_parse_error.type = EINI_INVALTK;
                ini_parse_error.row  = string_count;
                break;
            }
            else
                continue;
        case SECTION_OPEN_BRACKET:
        case SECTION_CLOSE_BRACKET:
            // check if token has leading trash before section/subsection
            //      "error: bad section/subsection syntax at ___ row. leading trash"
            // find close bracket
            //   if (found)
            //      check if string has tailing trash after section/subsection
            //      switch (string)
            //      case (comment):
            //          break;
            //      case (trash):
            //          "error: bad section/subsection syntax at ___ row. tailing trash"
            //      default:
            //          break;
            //      trim token
            //      lower token
            //      validate token with section_alphabet
            //      if (!valid)
            //          "error: bad section/subsection syntax at ___ row. invalid section name."
            //      check depth, if subsection
            //      if (too deep)
            //          "error: bad subsection syntax at ___ row. too deep."
            //      check section/subsection in INI
            //      if (found)
            //!         set this section/subsection
            //      else
            //!         add new section/subsection
            //   else
            //      "error: bad section/subsection syntax at ___ row. unclosed bracket."
        case PROPERTY_DELIMITER:
        case PROPERTY_DELIMITER_ALIAS:
            // 
            // 
            // 
            // 
            // 
        default:
            break;
        }
    }

    fclose(file);


    // TODO: place for error goto labels
}

#pragma endregion
