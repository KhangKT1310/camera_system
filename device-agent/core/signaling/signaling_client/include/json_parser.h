/**
 * @file json_parser.h
 * @brief Zero-Dependency C17 AST JSON Parser Interface for WebRTC Signaling.
 */

#ifndef CAMERA_SYSTEM_JSON_PARSER_H
#define CAMERA_SYSTEM_JSON_PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define JSON_MAX_DEPTH 32
#define JSON_MAX_TOKENS 256

/**
 * @brief JSON Node Type Classification.
 */
typedef enum {
    JSON_TYPE_NULL = 0,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT
} json_type_t;

/**
 * @brief Linked-list AST Node structure representing a parsed JSON element.
 */
typedef struct json_node {
    json_type_t type;            /**< Type of the JSON element. */
    char *key;                   /**< Key string allocated on heap if parent is object. */
    char *val_string;            /**< String value (unescaped) if type is JSON_TYPE_STRING. */
    double val_number;           /**< Numeric value if type is JSON_TYPE_NUMBER. */
    bool val_bool;               /**< Boolean value if type is JSON_TYPE_BOOLEAN. */
    struct json_node *children;  /**< Pointer to head of child AST node list. */
    struct json_node *next;      /**< Pointer to next sibling AST node in object/array. */
} json_node_t;

/**
 * @brief Parses a raw JSON string into a linked-list AST node tree.
 * 
 * Supports string unescaping, Unicode \u00XX encoding, array parsing, and strict trailing garbage rejection.
 * 
 * @param[in] json Raw JSON input string. Must not be NULL.
 * 
 * @return Pointer to root json_node_t on success, or NULL on parse failure.
 * 
 * @note Memory Ownership: Caller owns returned AST tree and MUST call json_free() when finished.
 */
json_node_t *json_parse(const char *json);

/**
 * @brief Recursively frees a JSON AST node tree and all associated memory.
 * 
 * @param[in] node Pointer to root AST node. If NULL, operation is a no-op.
 */
void json_free(json_node_t *node);

/**
 * @brief Searches a JSON Object node for a key and returns the matching child value AST node.
 * 
 * @param[in] object_node Target JSON Object node. Must be of type JSON_TYPE_OBJECT.
 * @param[in] key Key string to search for. Must not be NULL.
 * 
 * @return Pointer to matching value json_node_t, or NULL if key is not found or node is not an object.
 */
json_node_t *json_get_child(const json_node_t *object_node, const char *key);

/**
 * @brief Re-stringifies a JSON AST node into a clean JSON text payload.
 * 
 * Performs proper string escaping for quotes, backslashes, and control characters (< 0x20) into \u00XX.
 * 
 * @param[in] node Target AST node to stringify. Must not be NULL.
 * @param[out] buf Destination character buffer to receive stringified text.
 * @param[in] max_len Maximum capacity of destination buffer in bytes.
 * 
 * @return Number of characters written on success, or negative error code on failure.
 */
int json_stringify(const json_node_t *node, char *buf, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_JSON_PARSER_H */
