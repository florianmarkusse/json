#ifndef TEST_INSIDE_STRUCT_DESERIALIZATION_H
#define TEST_INSIDE_STRUCT_DESERIALIZATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "json/types.h" // for FLO_JSON_CREATE_OBJECT, flo_json_string, flo...

typedef struct flo_InsideTestStruct {
    flo_json_string aa;
    flo_json_uint64 bb;
} flo_InsideTestStruct;

FLO_JSON_CREATE_OBJECT(flo_InsideTestStruct, flo_json_InsideTestStruct);

bool compareInsideTestStruct(flo_json_InsideTestStruct *a,
                             flo_json_InsideTestStruct *b);

#ifdef __cplusplus
}
#endif

#endif
