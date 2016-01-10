#pragma once

#include <stdint.h>
#include <stdlib.h>

// Callback type
typedef int (*IpfCallback) (uint8_t *ipf, size_t size, char *archive, char *filename, void *userdata);

// Prototypes
int ipf_read (uint8_t *ipf, size_t size, IpfCallback callback, void *userdata);
