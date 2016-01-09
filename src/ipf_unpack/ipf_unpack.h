#pragma once

// === Includes ===
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// === Types ===
typedef enum {
    ACTION_ENCRYPT,
    ACTION_DECRYPT,
    ACTION_EXTRACT,
}   PackAction;

// === Prototypes ===
void generate_keys (uint32_t *keys);
void update_keys (uint32_t *keys, char b);
void decrypt (uint32_t *keys, uint8_t *buffer, size_t size);
void encrypt (uint32_t *keys, uint8_t *buffer, uint32_t size);
int read_ipf (uint8_t *ipf, size_t size, PackAction action);

// === Defines ===
#ifndef LOBYTE
#define LOBYTE(w) ((uint8_t)(w))
#endif

#define BYTEN(x, n) (* ((uint8_t*)& (x)+n))
#define BYTE3(x)    BYTEN (x,  3)
