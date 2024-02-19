# Zero-allocation JSON parser & (de)serializer
This project contains the serializing and deserializing of JSON according to a
schema. Note that it is not possible to parse a generic JSON input, a schema
must always be passed. 

Moreover, the project requires no dependencies other than those pre-installed
on a simple UNIX system. Additionally, the library functions without allocating
any memory, except when using the serializer to store the intermediate buffer.

To get started:
1. Clone the repository: 
```sh
git clone git@github.com:florianmarkusse/json.git
```
2. Install the required (dev-)dependencies, should be a no-op if skipping the development dependencies:
```sh
./install-dependencies.sh # or,
./install-dev-dependencies.sh
```
3. Run the build script:
```sh
./build.sh -h
```

## Features
Currently supported data types:
- `string`
- `bool`
- `uint64_t`
- `object`
- array of:
    - `string`
    - `bool`
    - `uint64_t`
    - `object`

the processing of negative/floating point numbers or heterogeneous arrays is
currently **not** supported.

## Quick Example
Below is an example of how to use the `json` tool. Also have a look at
`json/example`.

```c
#include "util/log.h"           // for FLO_INFO, FLO_FLUSH_AFTER, FLO_LOG_C...
#include "util/memory/arena.h"  // for flo_arena
#include "util/text/string.h"   // for FLO_STRING, flo_string, FLO_STRING_LEN
#include "util/types.h"         // for flo_char_d_a, flo_string_max_a
#include "json/deserialize.h"   // for flo_json_parse_root, FLO_DESERIALIZA...
#include "json/serialize.h"     // for flo_json_append
#include "json/struct-member.h" // for FLO_DESERIALIZE_OBJECT, FLO_JSON_CRE...
#include "json/types.h"         // for flo_json_void, FLO_JSON_CREATE_OBJECT
#include <stddef.h>             // for ptrdiff_t
#include <stdlib.h>             // for malloc

// The struct that we want to (de)serialize
typedef struct {
    // Note that primitive JSON data types are already predefined.
    flo_json_string text;
    flo_json_uint64 status;
} flo_Example;
// Wrap it in a JSON object.
FLO_JSON_CREATE_OBJECT(flo_Example, flo_json_Example);

// Create an array of the JSON object keys holding their type and offset into
// its parent object.
flo_json_schema keyBuffer[] = {
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_Example, flo_Example, text),
    FLO_JSON_CREATE_SIMPLE_KEY(flo_json_Example, flo_Example, status),
};
FLO_JSON_CREATE_KEYS_ARRAY(keyBuffer, flo_json_ExampleKeys);

flo_string exampleJSON =
    FLO_STRING("{\"text\": \n \"Request received succesfully.\"         , "
               "\"status\": \t  \t\t      35          }     ");

int main() {
    flo_json_Example response = {0};
    // Create a root JSON object from which the parser will work.
    flo_json_schema root = FLO_JSON_CREATE_ROOT(
        FLO_DESERIALIZE_OBJECT, &flo_json_ExampleKeys, flo_json_Example);

    // Create structure to hold information about possible parse failures.
    // The keyPath is the path from the root object to the key where the parser
    // failed.
    flo_string keyPathBuffer[8];
    flo_string_max_a keyPath = {.cap = 8, .len = 0, .buf = keyPathBuffer};

    flo_json_deserializationResult result = flo_json_parse_root(
        (flo_json_void *)&response, &keyPath, exampleJSON, root);
    if (result != FLO_DESERIALIZATION_SUCCESS) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR(FLO_STRING("Deserialization failed!!!\n"));
        }
        // Can check here what key failed and how based on the result and the
        // keyPath variable.
        return 1;
    }

    // Serializing through the library requires an arena to allocate more space
    // for the dynamic char array if needed. Can easily be replaced by a
    // different structure if preferred.
    flo_char_d_a serialized = {0};
    char *begin = malloc(1 << 10);
    flo_arena arena = (flo_arena){
        .beg = begin, .cap = 1 << 10, .end = begin + (ptrdiff_t)(1 << 10)};
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        // Printing as JSON object.
        flo_json_append(&serialized, (flo_json_void *)&response, root, &arena);
        FLO_INFO(FLO_STRING_LEN(serialized.buf, serialized.len), FLO_NEWLINE);

        // Or printing single field as example.
        FLO_INFO(FLO_STRING("the value of status is: "));
        FLO_INFO(response.value.status.value, FLO_NEWLINE);
    }

    return 0;
}
```

## build.sh 
This is a build script intended to make it easier to rebuild your
code. It is possible to build the code without it, this is done by invoking
CMake directly.

## install-dependencies.sh
This script installs the required depencencies for the project. Note that every
UNIX installation should have these installed already.

## install-dev-dependencies.sh
This script installs the required development depencencies for the project.
Note that for most people this will merely install the tools for fuzzing and
the others should already be present.

## Fuzzing
For fuzzing instructions, please refer to `json/fuzzing/README.md`
