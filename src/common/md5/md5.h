#pragma once

#include <stdio.h>
#include <stdint.h>

// Size of MD5
#define MD5_SIZE 32

// ---------- Includes ------------
char * MD5_string (char *string);
void   MD5_stringEx (char *string, char *result);
char * MD5_buffer (uint8_t *buffer, size_t size);
void   MD5_bufferEx (uint8_t *buffer, size_t size, char *result);
char * MD5_file   (FILE *file);
