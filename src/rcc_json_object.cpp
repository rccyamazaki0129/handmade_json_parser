#include "rcc_json_object.h"
#include <string.h>

/**
 * @brief Retrieve the JSON value associated with a given key from a JSON object.
 * 
 * This function initiates a search for a specified JSON key within the provided JSON object structure.
 * If the key is found, the associated value is returned. Otherwise, it returns an invalid JSON value.
 * 
 * @param JsonObject The JSON object in which to search for the key.
 * @param Key A string representing the JSON key to search for.
 * @return A JSON value associated with the key. If the key isn't found, the returned value's type 
 *         is set to JSON_TYPE_INVALID.
 */
json_value getJsonValue(json_object JsonObject, const char* Key)
{
    json_value Result;
    Result.Type = JSON_TYPE_INVALID;
    json_member* JsonMember = JsonObject.First;

    // Return invalid result if the starting member is null.
    if (JsonMember == nullptr) {
        return Result;
    }

    Result = getJsonValue(JsonMember, Key);
    return Result;
}

/**
 * @brief Retrieve the JSON value associated with a given key.
 * 
 * This function searches recursively through a JSON member structure to find a key and returns
 * its associated value. If the key is not found, it returns a JSON value with type set to 
 * JSON_TYPE_INVALID.
 * 
 * @param JsonMember Pointer to the starting JSON member.
 * @param Key String representing the JSON key to search for.
 * @return JSON value associated with the key or an invalid JSON value if the key isn't found.
 */
json_value getJsonValue(json_member* JsonMember, const char* Key)
{
    json_value Result;
    Result.Type = JSON_TYPE_INVALID;

    // Return invalid result if the starting member is null.
    if (JsonMember == nullptr) {
        return Result;    
    }

    json_member* TargetMember = JsonMember;

    while (TargetMember) {
        // Check if the current member's key matches the target key.
        if (strcmp(TargetMember->Key, Key) == 0) {
            Result = TargetMember->Value;
            break;
        }
        
        // If the current member's value type is another member, then search it recursively.
        if (TargetMember->Value.Type == JSON_TYPE_MEMBER) {
            Result = getJsonValue(TargetMember->Value.Child, Key);
            
            // If we find a match in the recursive search, break out.
            if (Result.Type != JSON_TYPE_INVALID) {
                break;
            }
        }

        // Move on to the next member in the list.
        TargetMember = TargetMember->Next;
    }

    return Result;
}

/**
 * @brief Retrieve the size of a JSON value array.
 *
 * This function returns the size of a JSON array type. If the provided
 * JSON value is not an array, it logs an error and returns -1.
 *
 * @param JsonValue The JSON value for which the size is to be retrieved.
 * @return Size of the JSON array if valid; -1 otherwise.
 */
inline int64_t getJsonValueArraySize(json_value JsonValue)
{
    if (JsonValue.Type != JSON_TYPE_ARRAY) {
        logOutput("Only JSON_TYPE_ARRAY can be passed to getJsonValueArraySize() as an argment.");
        return -1;
    }

    int64_t Result = JsonValue.Array.Size;
    return Result;
}

/**
 * @brief Retrieve a member from a JSON value array at a given index.
 *
 * This function returns the json_member present at the specified index
 * of a JSON array type. If the provided JSON value is not an array,
 * it logs an error and returns an uninitialized json_member.
 *
 * @note The returned json_member is uninitialized if the function encounters an error.
 *
 * @param JsonValue The JSON value array from which the member is to be retrieved.
 * @param Index The index at which the member is located in the array.
 * @return The json_member at the specified index if valid; uninitialized json_member otherwise.
 */
inline json_member getJsonValueArrayMember(json_value JsonValue, int32_t Index)
{
    json_member Result;

    if (JsonValue.Type != JSON_TYPE_ARRAY) {
        logOutput("Only JSON_TYPE_ARRAY can be passed to getJsonValueArrayMember() as an argment.");
        return Result;
    }

    Result = *(JsonValue.Array.Head[Index].Child);
    return Result;
}

/**
 * @brief Adds a new JSON member with the specified key and string value to a JSON object.
 * 
 * If the provided JSON object is empty, a new member is set as the first member.
 * Otherwise, the new member is appended to the end of the existing member list.
 * 
 * @param JsonObject Pointer to the target JSON object to which the member should be added.
 * @param Key The key for the new JSON member.
 * @param String The string value for the new JSON member.
 */
void addJsonMember(json_object* JsonObject, const char* Key, const char* String)
{
    if (Key == '\0') {
        logOutput("Key is not specified.");
        return;
    }

    // Generate new json_member
    json_member* NewMember = (json_member*)malloc(sizeof(json_member));

    // Set `Key` and `String`
    setJsonMemberValue(NewMember, Key, String);

    // If JsonObject is empty, set the new member as the first member
    if (JsonObject->First == nullptr) {
        JsonObject->First = NewMember;
        return;
    }

    // If JsonObject already has members, append the new member to the end
    json_member* CurrentMember = JsonObject->First;
    while (CurrentMember->Next != nullptr) {
        CurrentMember = CurrentMember->Next;
    }
    CurrentMember->Next = NewMember;
}

/**
 * @brief Adds a new JSON member with the specified key and number to a JSON object.
 * 
 * If the provided JSON object is empty, a new member is set as the first member.
 * Otherwise, the new member is appended to the end of the existing member list.
 * 
 * @param JsonObject Pointer to the target JSON object to which the member should be added.
 * @param Key The key for the new JSON member.
 * @param Number The float64_t value for the new JSON member.
 */
void addJsonMember(json_object* JsonObject, const char* Key, float64_t Number)
{
    if (Key == '\0') {
        logOutput("Key is not specified.");
        return;
    }

    // Generate new json_member, and set `Key` and `Number`
    json_member* NewMember = (json_member*)malloc(sizeof(json_member));
    setJsonMemberValue(NewMember, Key, Number);

    // If JsonObject is empty, set the new member as the first member
    if (JsonObject->First == nullptr) {
        JsonObject->First = NewMember;
        return;
    }

    // If JsonObject already has members, append the new member to the end
    json_member* CurrentMember = JsonObject->First;
    while (CurrentMember->Next != nullptr) {
        CurrentMember = CurrentMember->Next;
    }
    CurrentMember->Next = NewMember;
}

/**
 * @brief Adds a new JSON member with the specified key and boolean to a JSON object.
 * 
 * If the provided JSON object is empty, a new member is set as the first member.
 * Otherwise, the new member is appended to the end of the existing member list.
 * 
 * @param JsonObject Pointer to the target JSON object to which the member should be added.
 * @param Key The key for the new JSON member.
 * @param Boolean The boolean value for the new JSON member.
 */
void addJsonMember(json_object* JsonObject, const char* Key, bool32_t Boolean)
{
    if (Key == '\0') {
        logOutput("Key is not specified.");
        return;
    }

    // Generate new json_member, and set `Key` and `Boolean`
    json_member* NewMember = (json_member*)malloc(sizeof(json_member));
    setJsonMemberValue(NewMember, Key, Boolean);

    // If JsonObject is empty, set the new member as the first member
    if (JsonObject->First == nullptr) {
        JsonObject->First = NewMember;
        return;
    }

    // If JsonObject already has members, append the new member to the end
    json_member* CurrentMember = JsonObject->First;
    while (CurrentMember->Next != nullptr) {
        CurrentMember = CurrentMember->Next;
    }
    CurrentMember->Next = NewMember;
}

/**
 * @brief Adds a new JSON member with the specified key and another Json member to a JSON object.
 * 
 * If the provided JSON object is empty, a new member is set as the first member.
 * Otherwise, the new member is appended to the end of the existing member list.
 * 
 * @param JsonObject Pointer to the target JSON object to which the member should be added.
 * @param Key The key for the new JSON member.
 * @param Child The Json member for the new JSON member.
 */
void addJsonMember(json_object* JsonObject, const char* Key, json_member* Child)
{
    if (Child == nullptr || Key == '\0') {
        logOutput("Child member is null, or key is not specified.");
        return;
    }

    // Generate new json_member, and set `Key` and `Child`
    json_member* NewMember = (json_member*)malloc(sizeof(json_member));
    setJsonMemberValue(NewMember, Key, Child);

    // If JsonObject is empty, set the new member as the first member
    if (JsonObject->First == nullptr) {
        JsonObject->First = NewMember;
        return;
    }

    // If JsonObject already has members, append the new member to the end
    json_member* CurrentMember = JsonObject->First;
    while (CurrentMember->Next != nullptr) {
        CurrentMember = CurrentMember->Next;
    }
    CurrentMember->Next = NewMember;
}

/**
 * @brief Adds a new JSON object with the specified key to a JSON object.
 * 
 * If the provided JSON object is empty, a new member is set as the first member.
 * Otherwise, the new member is appended to the end of the existing member list.
 * 
 * @param JsonObject Pointer to the target JSON object to which the member should be added.
 * @param Key The key for the new JSON member.
 * @param Child The Json object for the new JSON member.
 */
void addJsonMember(json_object* JsonObject, const char* Key, json_object* Child)
{
    if (Child->First == nullptr || Key == '\0') {
        logOutput("Child member is null, or key is not specified.");
        return;
    }

    addJsonMember(JsonObject, Key, Child->First);
}

/**
 * @brief Adds a new JSON member with the specified key and array of json_value to a JSON object.
 * 
 * If the provided JSON object is empty, a new member is set as the first member.
 * Otherwise, the new member is appended to the end of the existing member list.
 * 
 * @param JsonObject Pointer to the target JSON object to which the member should be added.
 * @param Key The key for the new JSON member.
 * @param ArrayHead The array for the new JSON member.
 * @param ArraySize The size of array for the new JSON member.
 */
void addJsonMember(json_object* JsonObject, const char* Key, json_value* ArrayHead, size_t ArraySize)
{
    if (ArrayHead == nullptr || Key == '\0') {
        logOutput("Key is not specified.");
        return;
    }

    // Generate new json_member, and set `Key`, `ArrayHead`, `ArraySize`
    json_member* NewMember = (json_member*)malloc(sizeof(json_member));
    setJsonMemberValue(NewMember, Key, ArrayHead, ArraySize);

    // If JsonObject is empty, set the new member as the first member
    if (JsonObject->First == nullptr) {
        JsonObject->First = NewMember;
        return;
    }

    // If JsonObject already has members, append the new member to the end
    json_member* CurrentMember = JsonObject->First;
    while (CurrentMember->Next != nullptr) {
        CurrentMember = CurrentMember->Next;
    }
    CurrentMember->Next = NewMember;
}

/**
 * @brief Adds a new JSON member with a null value to the specified JSON object.
 * 
 * This function creates a new JSON member with a null value and appends it to
 * the end of the linked list of members in the provided JSON object. If the JSON
 * object is empty (no members yet), the new member is set as the first member.
 * If an empty key is provided, an error message is logged, and no member is added.
 * 
 * @param JsonObject Pointer to the JSON object to which the new member will be added.
 * @param Key The key for the new JSON member.
 */
void addJsonMemberNull(json_object* JsonObject, const char* Key)
{
    // Check for an empty key and log an error message if necessary
    if (Key == '\0') {
        logOutput("Key is not specified.");
        return;
    }
    
    // Generate new json_member, and set `Key` and `Boolean`
    json_member* NewMember = (json_member*)malloc(sizeof(json_member));
    setJsonMemberValueNull(NewMember, Key);

    // If JsonObject is empty, set the new member as the first member
    if (JsonObject->First == nullptr) {
        JsonObject->First = NewMember;
        return;
    }

    // If JsonObject already has members, append the new member to the end
    json_member* CurrentMember = JsonObject->First;
    while (CurrentMember->Next != nullptr) {
        CurrentMember = CurrentMember->Next;
    }
    CurrentMember->Next = NewMember;
}

/**
 * @brief Recursively deletes a JSON member with the specified key from a linked list of members.
 * 
 * @param JsonMember The head of the linked list of members to search.
 * @param Key The key of the member to delete.
 * @return Returns true if a member with the specified key was found and deleted, false otherwise.
 * TODO: Keep deleting until all of the key is found.
 */
bool32_t deleteJsonMember(json_member* JsonMember, const char* Key)
{
    bool32_t Result = false;
    json_member* TargetMember = JsonMember;

    // Traverse the list of JSON members
    while (TargetMember->Next != nullptr) {
        // Check if the next member's key matches the target key
        if (strcmp(TargetMember->Next->Key, Key) == 0) {
            json_member* ToDelete = TargetMember->Next;

            // Adjust pointers to remove the member from the list
            TargetMember->Next = ToDelete->Next;

            // Free memory allocated for the member
            destroyJsonMember(ToDelete);
            // free(ToDelete);

            Result = true;
            break;
        }
        // If the next member has child members, search recursively
        else if (TargetMember->Next->Value.Type == JSON_TYPE_MEMBER) {
            Result = deleteJsonMember(TargetMember->Next->Value.Child, Key);
            if (Result) {
                break;
            }
        }

        // Move to the next member in the list
        TargetMember = TargetMember->Next;
    }

    return Result;
}

/**
 * @brief Deletes a JSON member with the specified key from a JSON object.
 * 
 * @param JsonObject Reference to the JSON object from which the member should be deleted.
 * @param Key The key of the member to delete.
 * @return Returns true if a member with the specified key was found and deleted, false otherwise.
 */
bool32_t deleteJsonMember(json_object& JsonObject, const char* Key)
{
    bool32_t Result = false;

    // Check if the JSON object has no members
    if (JsonObject.First == nullptr) {
        logOutput("[WARN] Nothing to delete");
        Result = true;
    }
    // Check if the first member's key matches the target key
    else if (strcmp(JsonObject.First->Key, Key) == 0) {
        json_member* Temp = JsonObject.First;

        // Update the head of the list to the next member
        JsonObject.First = Temp->Next;

        // Free memory allocated for the first member
        destroyJsonMember(Temp);
        // free(Temp);

        Result = true;
    }
    // If the first member's key doesn't match, search the rest of the list
    else {
        Result = deleteJsonMember(JsonObject.First, Key);
    }

    return Result;
}

/**
 * @brief Recursively destroys a JSON member and all of its child members.
 * 
 * The function traverses the linked list of JSON members starting from the given `JsonMember` 
 * and frees the memory for each member. If a member has child members, the function 
 * recursively destroys the child members as well.
 *
 * @param JsonMember The starting JSON member in the linked list to be destroyed.
 */
void destroyJsonMember(json_member* JsonMember)
{
    json_member* TargetMember = JsonMember;
    json_member* NextTarget = TargetMember->Next;

    while (TargetMember != nullptr) {
        // // If the current member has child members, destroy them recursively
        if (TargetMember->Value.Type == JSON_TYPE_MEMBER) {
            destroyJsonMember(TargetMember->Value.Child);
        }

        // Free the memory allocated for the current member
        char* KeyMemory = (char*)TargetMember->Key;
        free(KeyMemory);
        free(TargetMember);

        // Update pointers for the next iteration
        TargetMember = NextTarget;
        if (NextTarget != nullptr) {
            NextTarget = NextTarget->Next;
        }
        else {
            NextTarget = nullptr;
        }
    }
}

/**
 * @brief Destroys a JSON object and all of its child members.
 * 
 * The function traverses the linked list of JSON members starting from the given `JsonObject` 
 * and frees the memory for each member. If a member has child members, the function 
 * recursively destroys the child members as well.
 *
 * @param JsonObject The JSON object to be destroyed.
 */
void destroyJsonObject(json_object* JsonObject)
{
    if (JsonObject == nullptr) {
        return;
    }

    destroyJsonMember(JsonObject->First);
    JsonObject->First = nullptr;
}

/**
 * @brief Prints the content of a given JSON value.
 * 
 * This function displays the content of a given `JsonValue` based on its type.
 * It handles various JSON types like member, array, string, number, boolean, and null.
 * For JSON members and arrays, the function calls itself recursively to print nested structures.
 * 
 * @param JsonValue The JSON value to be printed.
 */
void printJsonValue(json_value JsonValue)
{
    switch (JsonValue.Type) {
        case JSON_TYPE_MEMBER: {
            // Print JSON member by dereferencing its child pointer
            json_member Child = *(JsonValue.Child);
            printJsonMember(Child);
            /* TODO: Handle proper indentation for nested structures */
        } break;
        case JSON_TYPE_ARRAY: {
            printf("[");
            // Traverse through each value in the JSON array and print it
            json_value* JsonArray = JsonValue.Array.Head;
            for (int32_t Index = 0; Index < JsonValue.Array.Size; Index++) {
                printJsonValue(*(JsonArray + Index));
                // Add a comma separator for all but the last element
                if (Index < JsonValue.Array.Size - 1) {
                    printf(", ");
                }
            }
            printf("]");
        } break;
        case JSON_TYPE_STRING: {
            printf("\"%s\"", JsonValue.String);
        } break;
        case JSON_TYPE_NUMBER: {
            if (isFractionalPartZero(JsonValue.Number)) {
                printf("%d", (int32_t)JsonValue.Number);
            }
            else {
                printf("%.4lf", JsonValue.Number);
            }
        } break;
        case JSON_TYPE_BOOLEAN: {
            printf("%s", JsonValue.Boolean ? "true" : "false");
        } break;
        case JSON_TYPE_NULL: {
            printf("null");
        } break;
        default: {
            logOutput("[ERROR] Invalid json_type found.");
        } break;
    }
}

/**
 * @brief Prints the content of a given JSON member.
 * 
 * This function displays the content of a given `Member` in the format of a JSON object.
 * It iterates through the linked list of JSON members and prints each key-value pair.
 * For values that are themselves JSON members or arrays, the function calls 
 * appropriate helper functions to handle the nested structures.
 * 
 * @param Member The JSON member to be printed.
 */
void printJsonMember(json_member Member)
{
    json_member* TargetMember = &Member;
    if (TargetMember == nullptr) {
        logOutput("This JSON member is inaccessible.");
        return;
    }

    printf("{");
    while (true) {
        printf("\"%s\" : ", TargetMember->Key);
        printJsonValue(TargetMember->Value);
        if (TargetMember->Next) {
            printf(", ");
            TargetMember = TargetMember->Next;
        }
        else {
            break;
        }
    }
    printf("}");
}

/**
 * @brief Prints the content of a given JSON object.
 * 
 * This function displays the content of the provided `Object`. If the object is 
 * empty, a log message is displayed. Otherwise, the content of the JSON object
 * is printed in a formatted manner.
 * 
 * @param Object The JSON object to be printed.
 */
void printJsonObject(json_object Object)
{
    if (Object.First == nullptr) {
        logOutput("This JSON object is empty.");
    }
    else if (!Object.IsValid) {
        logOutput("This JSON object is not valid.");
    }
    else {
        printJsonMember(*(Object.First));
        printf("\n");
    }
}

// local functions

static inline void setJsonMemberValue(json_member* Member, const char* Key, const char* String)
{
    Member->Key = copyString(Key);
    Member->Value.Type = JSON_TYPE_STRING;
    Member->Value.String = copyString(String);
}

static inline void setJsonMemberValue(json_member* Member, const char* Key, float64_t Number)
{
    Member->Key = copyString(Key);
    Member->Value.Type = JSON_TYPE_NUMBER;
    Member->Value.Number = Number;
}

static inline void setJsonMemberValue(json_member* Member, const char* Key, bool32_t Boolean)
{
    Member->Key = copyString(Key);
    Member->Value.Type = JSON_TYPE_BOOLEAN;
    Member->Value.Boolean = Boolean;
}

static inline void setJsonMemberValue(json_member* Member, const char* Key, json_member* Child)
{
    Member->Key = copyString(Key);
    Member->Value.Type = JSON_TYPE_MEMBER;
    Member->Value.Child = Child;
}

static inline void setJsonMemberValue(json_member* Member, const char* Key, json_value* ArrayHead, size_t ArraySize)
{
    Member->Key = copyString(Key);
    Member->Value.Type = JSON_TYPE_ARRAY;
    Member->Value.Array.Head = (json_value*)malloc(sizeof(json_value) * ArraySize);
    memcpy(Member->Value.Array.Head, ArrayHead, sizeof(json_value) * ArraySize);
    Member->Value.Array.Size = ArraySize;
}

static inline void setJsonMemberValue(json_member* Member, const char* Key, json_object* ArrayHead, size_t ArraySize)
{
    Member->Key = copyString(Key);
    Member->Value.Type = JSON_TYPE_ARRAY;
    Member->Value.Array.Head = (json_value*)malloc(sizeof(json_value) * ArraySize);
    memcpy(Member->Value.Array.Head, &(ArrayHead->First->Value), sizeof(json_value) * ArraySize);
    Member->Value.Array.Size = ArraySize;
}

static inline void setJsonMemberValueNull(json_member* Member, const char* Key)
{
    Member->Key = copyString(Key);
    Member->Value.Type = JSON_TYPE_NULL;
}

static inline void setJsonMemberSibling(json_member* Member, json_member* Next)
{
    Member->Next = Next;
}
