# JSON (de)serializer

This project contains the serializing and deserializing of JSON. Note that it
is not possible to parse a generic JSON input, a schema must always be passed. 

## Features

- string
- bool
- uint64_t
- object
- Array of:
    - string
    - bool
    - uint64_t
    - object


the processing of negative/floating point numbers and heterogeneous arrays is
currently **not** supported yet.
