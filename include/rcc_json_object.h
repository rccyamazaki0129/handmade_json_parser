#ifndef RCC_JSON_OBJECT_H_
#define RCC_JSON_OBJECT_H_

#include "rcc_common.h"

// JSON members declaration
enum json_type
{
    JSON_TYPE_INVALID = 0,
    JSON_TYPE_MEMBER,
    JSON_TYPE_ARRAY,
    JSON_TYPE_STRING,
    JSON_TYPE_NUMBER,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NULL,
};

// forward declarations to use struct's pointer
typedef struct json_value json_value;
typedef struct json_member json_member;

struct json_value
{
    json_type Type;
    union {
        const char* String;
        float64_t Number;
        bool32_t Boolean;
        json_member* Child;
        struct {
            json_value* Head;
            size_t Size;
        } Array;
    };

    json_value() {
        Type = JSON_TYPE_INVALID;
        String = '\0';
    }
};

struct json_member
{
    const char* Key; // Key to access json_value
    json_value Value; // Value can hold any type of data
    json_member* Next; // Pointer to next json_member

    json_member() {
        Key = nullptr;
        Value = json_value();
        Next = nullptr;
    }
};

struct json_object
{
    json_member* First;
    bool32_t IsValid;
    json_object() {
        First = nullptr;
        IsValid = true;
    }
};

json_value getJsonValue(json_object JsonMember, const char* Key);
json_value getJsonValue(json_member* JsonMember, const char* Key);
inline int64_t getJsonValueArraySize(json_value JsonValue);
inline json_member getJsonValueArrayMember(json_value JsonValue, int32_t Index);
void addJsonMember(json_object* JsonObject, const char* Key, const char* String);
void addJsonMember(json_object* JsonObject, const char* Key, float64_t Number);
void addJsonMember(json_object* JsonObject, const char* Key, bool32_t Boolean);
void addJsonMember(json_object* JsonObject, const char* Key, json_member* Child);
void addJsonMember(json_object* JsonObject, const char* Key, json_object* Child);
void addJsonMember(json_object* JsonObject, const char* Key, json_value* ArrayHead, size_t ArraySize);
void addJsonMemberNull(json_object* JsonObject, const char* Key);
bool32_t deleteJsonMember(json_member* JsonMember, const char* Key);
bool32_t deleteJsonMember(json_object& JsonObject, const char* Key);
void destroyJsonMember(json_member* JsonMember);
void printJsonMember(json_member Member);
void printJsonValue(json_value JsonValue);
void printJsonObject(json_object Object);

// local functions
static inline void setJsonMemberValue(json_member* Member, const char* Key, const char* String);
static inline void setJsonMemberValue(json_member* Member, const char* Key, float64_t Number);
static inline void setJsonMemberValue(json_member* Member, const char* Key, bool32_t Boolean);
static inline void setJsonMemberValue(json_member* Member, const char* Key, json_member* Child);
static inline void setJsonMemberValue(json_member* Member, const char* Key, json_value* ArrayHead, size_t ArraySize);
static inline void setJsonMemberValueNull(json_member* Member, const char* Key);
static inline void setJsonMemberSibling(json_member* Member, json_member* Next);




#endif
