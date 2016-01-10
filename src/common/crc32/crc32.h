#pragma once

#include <stdint.h>

// Global shared variables
extern uint32_t CRC32_m_tab[];

// Prototypes
uint32_t compute_crc32 (uint32_t crc, char b);