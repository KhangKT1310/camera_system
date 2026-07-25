#ifndef CAMERA_SYSTEM_JSON_PARSER_H
#define CAMERA_SYSTEM_JSON_PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define JSON_MAX_TOKENS 256
#define JSON_MAX_DEPTH 8

typedef enum {
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_STRING,
    JSON_TYPE_NUMBER,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NULL
} json_type_t;

typedef struct json_node {
    json_type_t type;
    char *key;
    char *val_string;
    double val_number;
    bool val_bool;

    struct json_node *children;
    struct json_node *next;
} json_node_t;

/**
 * @brief Parses a raw JSON string into a structured AST node tree with bounds checking.
 * Rejects trailing garbage after root element.
 * @param json_str Target raw JSON string.
 * @return Pointer to root AST node, or NULL on parse error.
 */
json_node_t *json_parse(const char *json_str);

/**
 * @brief Frees a parsed JSON AST node tree.
 * @param root Root node of tree.
 */
void json_free(json_node_t *root);

/**
 * @brief Searches a JSON object node for a child node with matching key.
 * @param object_node JSON object node.
 * @param key Key name to look up.
 * @return Pointer to child node, or NULL if not found.
 */
json_node_t *json_get_child(const json_node_t *object_node, const char *key);

/**
 * @brief Serializes a JSON AST node tree back to string format.
 * @param node JSON node to stringify.
 * @param buf Output buffer pointer.
 * @param max_len Buffer size in bytes.
 * @return Number of characters written, or negative error on failure.
 */
int json_stringify(const json_node_t *node, char *buf, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_JSON_PARSER_H */
