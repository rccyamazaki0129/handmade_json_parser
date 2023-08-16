#include "rcc_common.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

/**
 * @brief Outputs a log message to the console.
 * 
 * This function prints the provided message to the standard output (console).
 * If the provided message is a null pointer, nothing will be printed.
 *
 * @param Message The message string to be logged. Can be null.
 */
inline void logOutput(const char* Message)
{
    if (Message) {
        printf("%s\n", Message);
    }
}

/**
 * @brief Creates a copy of the given string.
 * 
 * This function allocates a new block of memory and copies the content 
 * of the `Src` string into it. It's the caller's responsibility to 
 * free the memory allocated for the returned string.
 *
 * @param Src Pointer to the source string to be copied.
 * @return Pointer to the newly allocated string containing the copied content,
 *         or NULL if the memory allocation fails or if `Src` is NULL.
 */
char* copyString(const char* Src)
{
    if (Src == NULL) {
        return NULL;
    }

    // Calculate size of Src string including the null terminator
    size_t KeySize = 0;
    while (Src[KeySize] != '\0') {
        KeySize++;
    }
    KeySize++;

    // Allocate memory
    char* Dest = (char*)malloc(sizeof(char) * KeySize);
    if (Dest == NULL) {
        // Memory allocation failed
        return NULL;
    }

    // Copy string including the null terminator
    strcpy(Dest, Src);

    return Dest;
}

/**
 * @brief Checks if the provided character is a whitespace character.
 * 
 * This function determines whether the given character is one of the typical whitespace
 * characters: space (' '), newline ('\n'), tab ('\t'), or carriage return ('\r').
 *
 * @param Character The character to be checked.
 * @return Returns true if the character is a whitespace character, false otherwise.
 */
inline bool32_t isWhiteSpace(char Character)
{
    return Character == ' ' || Character == '\n' || Character == '\t' || Character == '\r';
}

/**
 * @brief Checks if the provided character is a numerical digit (0-9).
 * 
 * This function determines whether the given character is one of the ten decimal digits.
 *
 * @param Character The character to be checked.
 * @return Returns true if the character is a numerical digit, false otherwise.
 */
inline bool32_t isNumber(char Character)
{
    return (Character == '0' || Character == '1' || Character == '2' || Character == '3' || Character == '4'
         || Character == '5' || Character == '6' || Character == '7' || Character == '8' || Character == '9');
}


/**
 * @brief Checks if the fractional part of a floating point number is 0.
 * 
 * This function determines whether the fractional part of a given floating point number is 0.
 *
 * @param number The floating point number to be checked.
 * @return Returns true if the fractional part is 0, false otherwise.
 */
inline bool32_t isFractionalPartZero(float64_t Number)
{
    float64_t IntegerPart;
    float64_t FractionalPart = modf(Number, &IntegerPart);

    // Check if fractionalPart is almost zero. Using a threshold for floating point precision issues.
    return fabs(FractionalPart) < 1e-9;
}