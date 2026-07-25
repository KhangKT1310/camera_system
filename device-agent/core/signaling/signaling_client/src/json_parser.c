#include "json_parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void skip_whitespace(const char **p) {
    while (**p && ((unsigned char)**p <= 32 || **p == ' ' || **p == '\t' || **p == '\n' || **p == '\r')) {
        (*p)++;
    }
}

static char *parse_json_string(const char **p) {
    if (**p != '"') return NULL;
    (*p)++; /* skip opening quote */

    size_t cap = 128;
    size_t len = 0;
    char *buf = (char *)malloc(cap);
    if (!buf) return NULL;

    while (**p && **p != '"') {
        if (len + 4 >= cap) {
            cap *= 2;
            char *new_buf = (char *)realloc(buf, cap);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }

        if (**p == '\\') {
            (*p)++;
            switch (**p) {
                case '"': buf[len++] = '"'; break;
                case '\\': buf[len++] = '\\'; break;
                case '/': buf[len++] = '/'; break;
                case 'b': buf[len++] = '\b'; break;
                case 'f': buf[len++] = '\f'; break;
                case 'n': buf[len++] = '\n'; break;
                case 'r': buf[len++] = '\r'; break;
                case 't': buf[len++] = '\t'; break;
                case 'u': {
                    (*p)++;
                    uint32_t val = 0;
                    for (int i = 0; i < 4 && **p; i++, (*p)++) {
                        char c = **p;
                        val <<= 4;
                        if (c >= '0' && c <= '9') val |= (uint32_t)(c - '0');
                        else if (c >= 'a' && c <= 'f') val |= (uint32_t)(c - 'a' + 10);
                        else if (c >= 'A' && c <= 'F') val |= (uint32_t)(c - 'A' + 10);
                    }
                    (*p)--; /* compensate increment loop */
                    buf[len++] = (char)(val & 0x7F);
                    break;
                }
                default: buf[len++] = **p; break;
            }
        } else {
            buf[len++] = **p;
        }
        (*p)++;
    }

    if (**p == '"') (*p)++;
    buf[len] = '\0';
    return buf;
}

static json_node_t *parse_value(const char **p, int depth, int *token_count);

static json_node_t *parse_object(const char **p, int depth, int *token_count) {
    if (**p != '{') return NULL;
    (*p)++; /* skip '{' */

    json_node_t *head = NULL;
    json_node_t *tail = NULL;

    skip_whitespace(p);
    if (**p == '}') {
        (*p)++;
        json_node_t *node = (json_node_t *)calloc(1, sizeof(json_node_t));
        if (node) node->type = JSON_TYPE_OBJECT;
        return node;
    }

    while (**p && **p != '}') {
        skip_whitespace(p);
        if (**p != '"') {
            json_free(head);
            return NULL;
        }

        char *key = parse_json_string(p);
        if (!key) {
            json_free(head);
            return NULL;
        }

        skip_whitespace(p);
        if (**p != ':') {
            free(key);
            json_free(head);
            return NULL;
        }
        (*p)++; /* skip ':' */

        json_node_t *child = parse_value(p, depth + 1, token_count);
        if (!child) {
            free(key);
            json_free(head);
            return NULL;
        }
        child->key = key;

        if (!head) {
            head = child;
            tail = child;
        } else {
            tail->next = child;
            tail = child;
        }

        skip_whitespace(p);
        if (**p == ',') {
            (*p)++;
        } else if (**p == '}') {
            break;
        } else {
            json_free(head);
            return NULL;
        }
    }

    if (**p == '}') (*p)++;

    json_node_t *obj_node = (json_node_t *)calloc(1, sizeof(json_node_t));
    if (!obj_node) {
        json_free(head);
        return NULL;
    }
    obj_node->type = JSON_TYPE_OBJECT;
    obj_node->children = head;
    return obj_node;
}

static json_node_t *parse_value(const char **p, int depth, int *token_count) {
    skip_whitespace(p);
    if (!**p || depth > JSON_MAX_DEPTH || (*token_count) >= JSON_MAX_TOKENS) {
        return NULL;
    }

    (*token_count)++;

    if (**p == '{') {
        return parse_object(p, depth, token_count);
    } else if (**p == '"') {
        char *str = parse_json_string(p);
        if (!str) return NULL;
        json_node_t *node = (json_node_t *)calloc(1, sizeof(json_node_t));
        if (!node) { free(str); return NULL; }
        node->type = JSON_TYPE_STRING;
        node->val_string = str;
        return node;
    } else if (isdigit((unsigned char)**p) || **p == '-') {
        char *endptr = NULL;
        double val = strtod(*p, &endptr);
        if (endptr && endptr != *p) {
            *p = endptr;
            json_node_t *node = (json_node_t *)calloc(1, sizeof(json_node_t));
            if (!node) return NULL;
            node->type = JSON_TYPE_NUMBER;
            node->val_number = val;
            return node;
        }
        return NULL;
    } else if (strncmp(*p, "true", 4) == 0) {
        *p += 4;
        json_node_t *node = (json_node_t *)calloc(1, sizeof(json_node_t));
        if (!node) return NULL;
        node->type = JSON_TYPE_BOOLEAN;
        node->val_bool = true;
        return node;
    } else if (strncmp(*p, "false", 5) == 0) {
        *p += 5;
        json_node_t *node = (json_node_t *)calloc(1, sizeof(json_node_t));
        if (!node) return NULL;
        node->type = JSON_TYPE_BOOLEAN;
        node->val_bool = false;
        return node;
    } else if (strncmp(*p, "null", 4) == 0) {
        *p += 4;
        json_node_t *node = (json_node_t *)calloc(1, sizeof(json_node_t));
        if (!node) return NULL;
        node->type = JSON_TYPE_NULL;
        return node;
    }

    return NULL;
}

json_node_t *json_parse(const char *json_str) {
    if (!json_str) return NULL;
    const char *p = json_str;
    int token_count = 0;
    return parse_value(&p, 0, &token_count);
}

void json_free(json_node_t *root) {
    while (root) {
        json_node_t *next = root->next;
        if (root->key) free(root->key);
        if (root->val_string) free(root->val_string);
        if (root->children) json_free(root->children);
        free(root);
        root = next;
    }
}

json_node_t *json_get_child(const json_node_t *object_node, const char *key) {
    if (!object_node || !key) return NULL;
    json_node_t *curr = object_node->children;
    while (curr) {
        if (curr->key && strcmp(curr->key, key) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}
