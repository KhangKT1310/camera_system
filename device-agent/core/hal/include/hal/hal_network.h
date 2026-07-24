#ifndef CAMERA_HAL_NETWORK_H
#define CAMERA_HAL_NETWORK_H

#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t interface_id;
    char name[32];
    hal_net_if_type_t type;
    char mac_address[18];
    bool link_up;
} hal_network_interface_info_t;

typedef struct {
    char ip_address[46];
    char netmask[46];
    char gateway[46];
    char dns_primary[46];
} hal_ip_config_t;

hal_status_t hal_list_network_interfaces(hal_network_interface_info_t *interfaces, size_t *count);
hal_status_t hal_get_interface_state(uint32_t interface_id, bool *link_up);
hal_status_t hal_get_ip_configuration(uint32_t interface_id, hal_ip_config_t *config);
hal_status_t hal_resolve_dns(const char *hostname, char *out_ip, size_t out_ip_size);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_NETWORK_H */
