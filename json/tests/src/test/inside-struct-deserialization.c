#include "test/inside-struct-deserialization.h"
#include "common.h"             // for compareJsonString, compareJsonUint64
#include "json/struct-member.h" // for FLO_JSON_CREATE_STRUCT_MEMBER, flo_S...

bool compareInsideTestStruct(flo_json_InsideTestStruct *a,
                             flo_json_InsideTestStruct *b) {
    FLO_COMMON_JSON_COMPARE(a, b);
    return compareJsonString(&a->value.aa, &b->value.aa) &&
           compareJsonUint64(&a->value.bb, &b->value.bb);
}

static flo_json_schema memberBuffer[] = {
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_InsideTestStruct,
                                  flo_InsideTestStruct, aa),
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_InsideTestStruct,
                                  flo_InsideTestStruct, bb)};
FLO_JSON_CREATE_KEYS_ARRAY(memberBuffer, insideTestStructMembers);
