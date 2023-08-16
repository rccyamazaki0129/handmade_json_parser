/* Common functions developed by Rei.Y */
#ifndef RCC_COMMON_H_
#define RCC_COMMON_H_

#include <stdlib.h>

typedef int32_t bool32_t;
typedef float float32_t;
typedef double float64_t;

inline void logOutput(const char* Message);
char* copyString(const char* Src);
inline bool32_t isWhiteSpace(char Character);
inline bool32_t isNumber(char Character);
inline bool32_t isFractionalPartZero(float64_t number);

#endif