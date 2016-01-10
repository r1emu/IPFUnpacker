/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

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
void keys_generate (uint32_t *keys);
void keys_update   (uint32_t *keys, char b);
void ipf_decrypt (uint8_t *buffer, size_t size);
void ipf_encrypt (uint8_t *buffer, size_t size);

// === Defines ===
#ifndef LOBYTE
#define LOBYTE(w) ((uint8_t)(w))
#endif

#define BYTEN(x, n) (* ((uint8_t*)& (x)+n))
#define BYTE3(x)    BYTEN (x,  3)
