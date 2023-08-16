#include "rcc_common.h"
#include "rcc_json_object.h"
#include "rcc_json_parser.h"

#include "rcc_common.cpp"
#include "rcc_json_object.cpp"
#include "rcc_json_parser.cpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

int32_t main(int32_t ArgCount, const char** Args)
{
    // Ensure at least one argument (JSON file path) is passed.
    if (ArgCount < 2) {
        logOutput("Input json file is not specified.");
        return 0;
    }

    FILE* InputJsonFile = fopen(Args[1], "r");
    if (InputJsonFile == NULL) {
        // Failed to open input JSON file
        logOutput("[ERROR] Failed to open input json file.");
        return -1;
    }

    // Determine the size of the JSON file for buffer allocation.
    int64_t InputJsonFileSize = 0;
    fseek(InputJsonFile, 0, SEEK_END);
    InputJsonFileSize = ftell(InputJsonFile);
    fseek(InputJsonFile, 0, SEEK_SET);

    // Allocate memory to store JSON file contents.
    char* InputJsonBuffer = (char*)malloc(sizeof(char) * InputJsonFileSize);
    if (InputJsonBuffer == NULL) {
        logOutput("[ERROR] Failed to allocate InputJsonBuffer.");
        return -1;
    }

    // Read the JSON file contents into the buffer.
    fread(InputJsonBuffer, 1, sizeof(char) * InputJsonFileSize, InputJsonFile);
    fclose(InputJsonFile);

    // Parse the JSON buffer.
    size_t BufferIndex = 0;
    json_object ParsedJsonObject = parseStringToJson(InputJsonBuffer, InputJsonFileSize, BufferIndex);
    if (ParsedJsonObject.IsValid) {
        logOutput("JSON parsing succeeded.");
    }
    else {
        logOutput("JSON parsing failed.");
    }

    // Clean up the buffer as it's no longer needed.
    free(InputJsonBuffer);

    printJsonObject(ParsedJsonObject);

    // Retrieve a JSON array value, and if found, process its members.
    json_value Pairs = getJsonValue(ParsedJsonObject, "pairs");
    if (Pairs.Type == JSON_TYPE_ARRAY) {
        size_t NumberOfPairs = getJsonValueArraySize(Pairs);
        json_member* EachPair = (json_member*)malloc(sizeof(json_member) * NumberOfPairs);

        // Process each pair member from the array.
        for (int32_t i = 0; i < NumberOfPairs; i++) {
            EachPair[i] = getJsonValueArrayMember(Pairs, i);
            json_value X0, Y0, X1, Y1;
            json_member Member = EachPair[i];

            // Extract specific values from the current pair member.
            X0 = getJsonValue(&Member, "x0");
            Y0 = getJsonValue(&Member, "y0");
            X1 = getJsonValue(&Member, "x1");
            Y1 = getJsonValue(&Member, "y1");
        }

        // Clean up the pairs array memory.
        free(EachPair);
    }
    
    // Cleanup and final logging.
    destroyJsonObject(&ParsedJsonObject);
    logOutput("Handmade Json Parser run successfully.");
    return 0;
}