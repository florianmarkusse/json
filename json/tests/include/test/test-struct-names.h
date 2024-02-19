#ifndef TEST_TEST_STRUCT_NAMES_H
#define TEST_TEST_STRUCT_NAMES_H

#ifdef __cplusplus
extern "C" {
#endif

/** This is just for testing comfort. Normally this would be defined per header
 * file for each struct.
 */

extern struct flo_json_schema_a insideTestStructMembers;
extern struct flo_json_schema_a oneLevelStructMembers;
extern struct flo_json_schema_a integrationStructMembers;
extern struct flo_json_schema_a stringStructMembers;
extern struct flo_json_schema_a structStructMembers;
extern struct flo_json_schema_a uint64StructMembers;

#ifdef __cplusplus
}
#endif

#endif
