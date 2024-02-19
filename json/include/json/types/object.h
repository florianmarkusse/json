#ifndef JSON_TYPES_OBJECT_H
#define JSON_TYPES_OBJECT_H

#include "parse-framework/parse.h" // for flo_parseStatus
#include "util/types.h"            // for flo_string_max_a
#include "json/deserialize.h"      // for flo_json_deserializationResult
#include "json/types.h"            // for flo_json_object
struct flo_json_schema_a;          // lines 8-8

flo_json_deserializationResult
flo_deserializeObject(flo_json_object *toDeserialize,
                      flo_string_max_a *nestedKeys, flo_parseStatus *ps,
                      struct flo_json_schema_a *keys);

#endif
