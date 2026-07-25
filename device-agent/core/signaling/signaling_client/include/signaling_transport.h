#ifndef CAMERA_SYSTEM_SIGNALING_TRANSPORT_H
#define CAMERA_SYSTEM_SIGNALING_TRANSPORT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int (*connect)(void *ctx, const char *uri);
    int (*send)(void *ctx, const void *data, size_t size);
    int (*close)(void *ctx);
} signaling_transport_ops_t;

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_SIGNALING_TRANSPORT_H */
