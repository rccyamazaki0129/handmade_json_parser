#include "rcc_common.h"
#include "rcc_json_object.h"
#include "rcc_json_parser.h"
#include "rcc_profiler.h"

#include "rcc_common.cpp"
#include "rcc_json_object.cpp"
#include "rcc_json_parser.cpp"
#include "rcc_profiler.cpp"

#include "listing_0065_haversine_formula.cpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TEST main for profiling
int32_t test(int32_t ArgCount, const char** Args)
{
    PROFILE_FUNC;

    // Ensure at least one argument (JSON file path) is passed.
    if (ArgCount < 2) {
        logOutput("Input json file is not specified.");
        return 0;
    }

    FILE* InputJsonFile;
    int64_t InputJsonFileSize;
    char* InputJsonBuffer;
    {
        PROFILE_BLOCK("JSON file read");

        InputJsonFile = fopen(Args[1], "r");
        if (InputJsonFile == NULL) {
            // Failed to open input JSON file
            logOutput("[ERROR] Failed to open input json file.");
            return -1;
        }

        // Determine the size of the JSON file for buffer allocation.
        InputJsonFileSize = 0;
        fseek(InputJsonFile, 0, SEEK_END);
        InputJsonFileSize = ftell(InputJsonFile);
        fseek(InputJsonFile, 0, SEEK_SET);

        // Allocate memory to store JSON file contents.
        InputJsonBuffer = (char*)malloc(sizeof(char) * InputJsonFileSize);
        if (InputJsonBuffer == NULL) {
            logOutput("[ERROR] Failed to allocate InputJsonBuffer.");
            return -1;
        }

        // Read the JSON file contents into the buffer.
        fread(InputJsonBuffer, 1, sizeof(char) * InputJsonFileSize, InputJsonFile);
        fclose(InputJsonFile);
    }

    size_t BufferIndex;
    json_object ParsedJsonObject;
    {
        PROFILE_BLOCK("JSON parse");

        // Parse the JSON buffer.
        BufferIndex = 0;
        ParsedJsonObject = parseStringToJson(InputJsonBuffer, InputJsonFileSize, BufferIndex);
        if (ParsedJsonObject.IsValid) {
            logOutput("JSON parsing succeeded.");
        }
        else {
            logOutput("JSON parsing failed.");
        }
    }
    
    {
        PROFILE_BLOCK("Cleanup JSON buffer");

        // Clean up the buffer as it's no longer needed.
        free(InputJsonBuffer);
    }

    {
        PROFILE_BLOCK("Harversine formula");

        // Retrieve a JSON array value, and if found, process its members.
        json_value Pairs = getJsonValue(ParsedJsonObject, "pairs");
        if (Pairs.Type == JSON_TYPE_ARRAY) {
            size_t NumberOfPairs = getJsonValueArraySize(Pairs);
            json_member* EachPair = (json_member*)malloc(sizeof(json_member) * NumberOfPairs);

                // Haversine distance average
                float64_t HaversineDistanceSum = 0.0;
                float64_t HaversineDistanceAverage = 0.0;

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

                // Compute Haversine formula.
                float64_t HaversineDistance = ReferenceHaversine(X0.Number, Y0.Number, X1.Number, Y1.Number, 6372.8);
                HaversineDistanceSum += HaversineDistance;
            }

            // Compute Haversine distance average.
            HaversineDistanceAverage = HaversineDistanceSum / NumberOfPairs;

            {
                PROFILE_BLOCK("Print message");

                printf("Pair count: %ld\n", NumberOfPairs);
                printf("Haversine distance average: %.16lf\n", HaversineDistanceAverage);

                {
                    PROFILE_BLOCK("Cleanup pairs array");

                    // Clean up the pairs array memory.
                    free(EachPair);
                }

                {
                    PROFILE_BLOCK("Compare result");

                    // Compare the result if reference average file is specified.
                    if (ArgCount >= 3) {
                        FILE* ReferenceAverageFile = fopen(Args[2], "rb");
                        if (ReferenceAverageFile == NULL) {
                            printf("[ERROR] Failed to open %s\n", Args[2]);
                            return -1;
                        }

                        // Get the last 8 bytes (refenrece average distance)
                        float64_t ReferenceDistanceAverage;
                        fseek(ReferenceAverageFile, -8, SEEK_END);
                        fread(&ReferenceDistanceAverage, 8, 1, ReferenceAverageFile);
                        printf("\n[Validation]\nReference distance average: %.16lf\n", ReferenceDistanceAverage);
                        printf("Diff: %.16lf\n", (ReferenceDistanceAverage - HaversineDistanceAverage));
                        fclose(ReferenceAverageFile);
                    }
                }
                
            }
        }
    }
    
    {
        PROFILE_BLOCK("Destroy JSON Object");

        // Cleanup and final logging.
        destroyJsonObject(&ParsedJsonObject);
    }

    logOutput("Handmade Json Parser run successfully.");

    return 0;
}

int32_t main(int32_t ArgCount, const char** Args)
{
    initializeProfiler();
 
    test(ArgCount, Args);
 
    finalizeProfiler();
    return 0;
}