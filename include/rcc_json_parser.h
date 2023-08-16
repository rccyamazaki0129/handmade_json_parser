#ifndef RCC_JSON_PARSER_H_
#define RCC_JSON_PARSER_H_

#include "rcc_json_object.h"
#include <stdlib.h>
#include <string.h>

#define JSON_TOKEN_STRING_SIZE 64

/* Token declaration */
enum json_token_type
{
    JSON_TOKEN_INVALID = 0,
    JSON_TOKEN_OBJECT_START,  // {
    JSON_TOKEN_OBJECT_END,    // }
    JSON_TOKEN_ARRAY_START,   // [
    JSON_TOKEN_ARRAY_END,     // ]
    JSON_TOKEN_COMMA,         // ,
    JSON_TOKEN_COLON,         // :
    JSON_TOKEN_STRING,
    JSON_TOKEN_NUMBER,
    JSON_TOKEN_BOOLEAN,
    JSON_TOKEN_NULL
};

struct json_token
{
    json_token_type Type;
    char String[JSON_TOKEN_STRING_SIZE];

    json_token() {
        Type = JSON_TOKEN_INVALID;
        memset(String, '\0', sizeof(char) * JSON_TOKEN_STRING_SIZE);
    }
};

json_token tokenizeString(const char* InputJsonBuffer, size_t &BufferIndex);
json_object parseStringToJson(const char* InputJsonBuffer, size_t InputJsonFileSize, size_t &BufferIndex);

#endif