#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//#include "ini/ini.h"

const char _comment_scanset[]  = " %*[#;]"; //!< whitespaces [#;]
const char _section_scanset[]  = " [ %[0-9a-zA-Z._] ]"; //!< whitespaces '[' whitespaces [0-9a-zA-Z._] whitespaces ']' 
const char _property_scanset[] = " %[0-9a-zA-Z_] %*[=:] %[^#;\r\n]"; //!< whitespaces [0-9a-zA-Z_] [=;] [^#;]

typedef struct rettok {
    char* key;
    char* value;
    char* section;
    bool hascomment;
} rettok;
rettok tokenize(char* str, char* key, char* value, char* section) {
    if (str == NULL ||
        key == NULL ||
        value == NULL)
        return;
    //int readed = -1;
    //puts(str);

    if (sscanf(str, _comment_scanset) == 1) {
        return (rettok) { .key = NULL, .value = NULL, .section = NULL, .hascomment = true };
    }
    if (sscanf(str, _section_scanset, section) == 1) {
        return (rettok) { .key = NULL, .value = NULL, .section = section, .hascomment = false };
    }
    if (sscanf(str, _property_scanset, key, value) == 2) {
        return (rettok) { .key = key, .value = value, .section = NULL, .hascomment = false };
    }
    if (sscanf(str, _property_scanset, key, value) == 1) {
        return (rettok) { .key = key, .value = "", .section = NULL, .hascomment = false };
    }
    return (rettok) { .key = NULL, .value = NULL, .section = NULL, .hascomment = false };
}

void print_rettok(rettok tok) {
    if (tok.hascomment == true) {
        printf("string has comment\n");
        return;
    }
    if (tok.section) {
        printf("\tsection[%lld]: \"%s\"\n", strlen(tok.section), tok.section);
        return;
    }
    if (tok.key || tok.value) {
        printf("k[%lld]: \"%20s\", v[%lld]: \"%s\"\n", strlen(tok.key), tok.key, strlen(tok.value), tok.value);
        return;
    }
    puts("invalid token!");
}

int main(void) {
    char buffer [256] = { '\0' };
    char key    [256] = { '\0' };
    char value  [256] = { '\0' };
    char section[256] = { '\0' };

    FILE* ini = fopen("test.ini", "r");
    if (!ini)
        return EXIT_FAILURE;

    while (fgets(buffer, 256, ini) != NULL) {
        rettok tok = tokenize(buffer, key, value, section);
        print_rettok(tok);
    }
    if (feof(ini))
        puts("End Of File reached!");

    fclose(ini);

    return EXIT_SUCCESS;
}