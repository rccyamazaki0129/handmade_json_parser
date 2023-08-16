#include "rcc_json_parser.h"
#include "stdio.h"

/**
 * @brief Tokenizes a JSON string, extracting one token at a time.
 *
 * This function scans the provided JSON buffer starting from the current BufferIndex and
 * extracts the next token it encounters. BufferIndex will be updated to point to the position
 * after the token.
 *
 * @param InputJsonBuffer Pointer to the buffer containing the JSON string to be tokenized.
 * @param BufferIndex Reference to the position in the buffer from which tokenization should start.
 *                    After the function call, it will be updated to the position after the extracted token.
 *
 * @return A json_token structure representing the token extracted. If the function encounters an 
 *         unexpected or invalid sequence in the JSON string, the returned token might be of an undefined type.
 *
 * @note The function will skip any whitespace characters (as determined by the isWhiteSpace function) before
 *       attempting to extract a token.
 */
json_token tokenizeString(const char* InputJsonBuffer, size_t &BufferIndex)
{
    json_token Result;

    // Skip white spaces.
    while (isWhiteSpace(InputJsonBuffer[BufferIndex])) {
        BufferIndex++;
    }

    switch (InputJsonBuffer[BufferIndex]) {
        case '{':{
            Result.Type = JSON_TOKEN_OBJECT_START;
            BufferIndex++;
        } break;
        case '}': {
            Result.Type = JSON_TOKEN_OBJECT_END;
            BufferIndex++;
        } break;
        case '[': {
            Result.Type = JSON_TOKEN_ARRAY_START;
            BufferIndex++;
        } break;
        case ']': {
            Result.Type = JSON_TOKEN_ARRAY_END;
            BufferIndex++;
        } break;
        case ',': {
            Result.Type = JSON_TOKEN_COMMA;
            BufferIndex++;
        } break;
        case ':': {
            Result.Type = JSON_TOKEN_COLON;
            BufferIndex++;
        } break;
        case '"': {
            // String detected.
            BufferIndex++;
            size_t StartIndex = BufferIndex;
            while (InputJsonBuffer[BufferIndex] != '"' && InputJsonBuffer[BufferIndex] != '\0') {
                BufferIndex++;
            }
            if (InputJsonBuffer[BufferIndex] == '\0') {
                // Invalid JSON format.
                return Result;
            }
            // Save only valid string in json_token.
            strncpy(Result.String, &InputJsonBuffer[StartIndex], BufferIndex - StartIndex);
            Result.String[BufferIndex - StartIndex] = '\0';
            Result.Type = JSON_TOKEN_STRING;
            BufferIndex++;
        } break;
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            // Number detected
            size_t StartIndex = BufferIndex;
            BufferIndex++;
            while (isNumber(InputJsonBuffer[BufferIndex]) || InputJsonBuffer[BufferIndex] == '.' ||
                InputJsonBuffer[BufferIndex] == 'e' || InputJsonBuffer[BufferIndex] == '-') {
                BufferIndex++;
            }
            if (InputJsonBuffer[BufferIndex] == '\0') {
                // Invalid JSON format.
                return Result;
            }
            // Save only valid string in json_token.
            strncpy(Result.String, &InputJsonBuffer[StartIndex], BufferIndex - StartIndex);
            Result.String[BufferIndex - StartIndex] = '\0';
            Result.Type = JSON_TOKEN_NUMBER;
        } break;
        case 't': {
            // Boolean detected (true)
            if (strncmp(&InputJsonBuffer[BufferIndex], "true", 4) == 0) {
                Result.Type = JSON_TOKEN_BOOLEAN;
                strncpy(Result.String, "true", 4);
                BufferIndex += 4;
            }
        } break;
        case 'f': {
            // Boolean detected (false)
            if (strncmp(&InputJsonBuffer[BufferIndex], "false", 5) == 0) {
                Result.Type = JSON_TOKEN_BOOLEAN;
                strncpy(Result.String, "false", 5);
                BufferIndex += 5;
            }
        } break;
        case 'n': {
            // null detected
            if (strncmp(&InputJsonBuffer[BufferIndex], "null", 4) == 0) {
                Result.Type = JSON_TOKEN_NULL;
                strncpy(Result.String, "null", 4);
                BufferIndex += 4;
            }
        } break;
    }
    return Result;
}

/**
 * @brief Parses a JSON string and returns the resulting JSON object.
 *
 * This function tokenizes the given input JSON string and constructs a 
 * corresponding json_object structure. If an error occurs during parsing, 
 * the function logs the error message and returns an invalid json_object.
 *
 * @param InputJsonBuffer The input string containing the JSON data.
 * @param InputJsonFileSize The size of the input JSON data.
 * @param BufferIndex The current position in the input buffer.
 * @return A json_object representing the parsed JSON data. If parsing fails, the IsValid member is set to false.
 *
 * @note The function may modify the BufferIndex parameter to indicate the current position in the buffer.
 */
json_object parseStringToJson(const char* InputJsonBuffer, size_t InputJsonFileSize, size_t &BufferIndex)
{
    json_object Result;

    while (BufferIndex < InputJsonFileSize) {
        json_token Token = tokenizeString(InputJsonBuffer, BufferIndex);

        if (Token.Type == JSON_TOKEN_INVALID) {
            logOutput("Failed to tokenize string.");
            break;
        }
        
        if (Token.Type == JSON_TOKEN_OBJECT_START) {
            while (Token.Type != JSON_TOKEN_OBJECT_END) {
                // parse key
                json_token KeyToken = tokenizeString(InputJsonBuffer, BufferIndex);
                if (KeyToken.Type != JSON_TOKEN_STRING) {
                    // key has to be JSON_TOKEN_STRING
                    logOutput("[ERROR] Invalid key has been found.");
                    Result.IsValid = false;
                    return Result;
                }

                // skip the colon
                json_token ColonToken = tokenizeString(InputJsonBuffer, BufferIndex);
                if (ColonToken.Type != JSON_TOKEN_COLON) {
                    // colon has to be here
                    logOutput("[ERROR] Colon is missing.");
                    Result.IsValid = false;
                    return Result;
                }

                // parsing value
                json_token ValueToken = tokenizeString(InputJsonBuffer, BufferIndex);
                switch (ValueToken.Type) {
                    case JSON_TOKEN_OBJECT_START: {
                        // logOutput("Object start");
                        BufferIndex--; // BufferIndex has point to `{` in order to parse JSON object
                        json_object Child = parseStringToJson(InputJsonBuffer, InputJsonFileSize, BufferIndex);
                        addJsonMember(&Result, KeyToken.String, &Child);
                    } break;
                    case JSON_TOKEN_ARRAY_START: {
                        // logOutput("Array start");
                        size_t LocalStoreBufferIndex = BufferIndex;
                        json_token ArrayToken = tokenizeString(InputJsonBuffer, BufferIndex);

                        // Calculate the size of array.
                        size_t ArraySize = 0;
                        while (ArrayToken.Type != JSON_TOKEN_ARRAY_END) {
                            switch (ArrayToken.Type) {
                                case JSON_TOKEN_OBJECT_START: {
                                    // tokenizer need `{` to detect JSON_TOKEN_OBJECT_START
                                    size_t ArrayBufferIndex = BufferIndex - 1;
                                    parseStringToJson(InputJsonBuffer, InputJsonFileSize, ArrayBufferIndex);
                                    BufferIndex = ArrayBufferIndex;
                                    ArraySize++;
                                } break;
                                case JSON_TOKEN_NUMBER:
                                case JSON_TOKEN_BOOLEAN:
                                case JSON_TOKEN_NULL:
                                case JSON_TOKEN_STRING: {
                                    ArraySize++;
                                } break;
                                case JSON_TOKEN_COMMA: {
                                    // Nothing to do.
                                } break;
                                default: {
                                    logOutput("[ERROR] Invalid token has been found in a array.");
                                    Result.IsValid = false;
                                    return Result;
                                }
                            }

                            ArrayToken = tokenizeString(InputJsonBuffer, BufferIndex);
                        }

                        // Restore BufferIndex
                        BufferIndex = LocalStoreBufferIndex;

                        // printf("Array size: %d\n", ArraySize);

                        // Allocate dynamic memory for json_value array
                        json_value* ValueArray = (json_value*)malloc(sizeof(json_value) * ArraySize);
                        size_t ValueArrayIndex = 0;
                        json_object TempObject;
                        
                        // FIXME: Calling tokenizeString() twice is bad.
                        ArrayToken = tokenizeString(InputJsonBuffer, BufferIndex);
                        while (ArrayToken.Type != JSON_TOKEN_ARRAY_END) {

                            switch (ArrayToken.Type) {
                                case JSON_TOKEN_OBJECT_START: {
                                    // tokenizer need `{` to detect JSON_TOKEN_OBJECT_START
                                    size_t ArrayBufferIndex = BufferIndex - 1;
                                    TempObject = parseStringToJson(InputJsonBuffer, InputJsonFileSize, ArrayBufferIndex);
                                    BufferIndex = ArrayBufferIndex;
                                    ValueArray[ValueArrayIndex].Type = JSON_TYPE_MEMBER;
                                    ValueArray[ValueArrayIndex].Child = TempObject.First;
                                    ValueArrayIndex++;
                                } break;
                                    // TODO: need to store value here
                                case JSON_TOKEN_NUMBER: {
                                    float64_t Number = atof(ArrayToken.String);
                                    ValueArray[ValueArrayIndex].Type = JSON_TYPE_NUMBER;
                                    ValueArray[ValueArrayIndex].Number = Number;
                                    ValueArrayIndex++;
                                } break;
                                case JSON_TOKEN_BOOLEAN: {
                                    bool32_t Boolean = strncmp(ArrayToken.String, "true", 4) == 0;
                                    ValueArray[ValueArrayIndex].Type = JSON_TYPE_BOOLEAN;
                                    ValueArray[ValueArrayIndex].Boolean = Boolean;
                                    ValueArrayIndex++;
                                } break;
                                case JSON_TOKEN_NULL: {
                                    ValueArray[ValueArrayIndex].Type = JSON_TYPE_NULL;
                                    ValueArrayIndex++;
                                } break;
                                case JSON_TOKEN_STRING: {
                                    ValueArray[ValueArrayIndex].Type = JSON_TYPE_STRING;
                                    ValueArray[ValueArrayIndex].String = copyString(ArrayToken.String);
                                    ValueArrayIndex++;
                                } break;
                                case JSON_TOKEN_COMMA: {
                                    // Nothing to do.
                                } break;
                                default: {
                                    logOutput("[ERROR] Invalid token has been found in a array.");
                                    Result.IsValid = false;
                                    return Result;
                                }
                            }
                            ArrayToken = tokenizeString(InputJsonBuffer, BufferIndex);
                        }
                        addJsonMember(&Result, KeyToken.String, ValueArray, ArraySize);
                        // Release json_value array.
                        free(ValueArray);
                    } break;
                    case JSON_TOKEN_STRING: {
                        // printf("String: %s\n", ValueToken.String);
                        addJsonMember(&Result, KeyToken.String, ValueToken.String);
                    } break;
                    case JSON_TOKEN_NUMBER: {
                        // printf("Number: %s\n", ValueToken.String);
                        float64_t Number = atof(ValueToken.String);
                        addJsonMember(&Result, KeyToken.String, Number);
                    } break;
                    case JSON_TOKEN_BOOLEAN: {
                        // printf("Boolean: %s\n", ValueToken.String);
                        bool32_t Boolean = strncmp(ValueToken.String, "true", 4) == 0;
                        addJsonMember(&Result, KeyToken.String, Boolean);
                    } break;
                    case JSON_TOKEN_NULL: {
                        // logOutput("NULL");
                        addJsonMemberNull(&Result, KeyToken.String);
                    } break;
                    default: {
                        logOutput("[ERROR] Invalid value found.");
                        Result.IsValid = false;
                        return Result;
                    }
                }

                // skip the comma
                json_token CommaToken = tokenizeString(InputJsonBuffer, BufferIndex);
                if (CommaToken.Type != JSON_TOKEN_COMMA) {
                    // Parsing has reached to the end of object.
                    return Result;
                }
            }
        }
    }

    return Result;
}
