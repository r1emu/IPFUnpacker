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

#include "ipf_unpack.h"
#include "dbg/dbg.h"
#include "crc32/crc32.h"
#include "fs/fs.h"
#include "zlib/zlib.h"
#include "md5/md5.h"
#include "ipf.h"
#include "ies.h"
#include <libgen.h>
#include <stdbool.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

char *default_decompressed_extensions[] = {"xml", "ies", "jpg", "png", "tga", "lua"};


void keys_generate (uint32_t *keys) 
{
    uint8_t password[20] = {0x6F, 0x66, 0x4F, 0x31, 0x61, 0x30, 0x75, 0x65, 0x58, 0x41, 0x3F, 0x20, 0x5B, 0xFF, 0x73, 0x20, 0x68, 0x20, 0x25, 0x3F};

    keys[0] = 0x12345678;
    keys[1] = 0x23456789;
    keys[2] = 0x34567890;

    for (int i = 0; i < sizeof(password); i++) {
        keys_update (keys, password[i]);
    }
}

void keys_update (uint32_t *keys, char b)
{
    keys[0] = compute_crc32 (keys[0], b);
    keys[1] = 0x8088405 * ((uint8_t) keys[0] + keys[1]) + 1;
    keys[2] = compute_crc32 (keys[2], BYTE3 (keys[1]));
}

void ipf_decrypt (uint8_t *buffer, size_t size)
{
    uint32_t keys[3];
    keys_generate (keys);
    size = ((size - 1) >> 1) + 1;

    for (int i = 0; i < size; i++, buffer += 2) {
        uint16_t v = (keys[2] & 0xFFFD) | 2;
        *buffer ^= (v * (v ^ 1)) >> 8;
        keys_update (keys, *buffer);
    }
}

void ipf_encrypt (uint8_t *buffer, size_t size)
{
    uint32_t keys[3];
    keys_generate (keys);
    size = ((size - 1) >> 1) + 1;

    for (int i = 0; i < size; i++, buffer += 2) {
        uint16_t v = (keys[2] & 0xFFFD) | 2;
        keys_update (keys, *buffer);
        *buffer ^= (v * (v ^ 1)) >> 8;
    }
}

typedef struct {
    FILE *output;
} IesParams;
static bool process_ies (IesTable *table, void *userdata)
{
    IesParams *params = (IesParams *) userdata;
    FILE *output = params->output;

    for (int colId = 0; colId < table->header->colsCount; colId++) {
        IesColumn *col = &table->columns[colId];
        fprintf (output, "%s", col->name);
        if (colId != table->header->colsCount - 1) {
            fprintf (output, ",");
        }
    }
    fprintf(output, "\n");

    for (int rowId = 0; rowId < table->header->rowsCount; rowId++) 
    {
        IesRow *row = &table->rows[rowId];
        for (int cellId = 0; cellId < row->cellsCount; cellId++)
        {
            IesCell(0, 0) *_cell = (void *) row->cells[cellId];
            IesCell(_cell->optSize, _cell->strSize) *cell = (void *) _cell;
            IesColumn *col = cell->col;

            switch (col->type) {
                case 0: {
                    if (cell->flt.value == (int) cell->flt.value) {
                        fprintf (output, "%d", (int) cell->flt.value);
                    } else {
                        fprintf (output, "%f", cell->flt.value);
                    }
                    if (cellId != row->cellsCount - 1) {
                        fprintf (output, ",");
                    }
                    // info ("\t[%d][%s] => %f", cellId, col->name, cell->flt.value);
                } break;

                case 1:
                case 2: {
                    fprintf (output, "\"%s\"", cell->str.value);
                    if (cellId != row->cellsCount - 1) {
                        fprintf (output, ",");
                    }
                    // info ("\t[%d][%s] => <%s>", cellId, col->name, cell->str.value);
                } break;
            }
        }
        fprintf(output, "\n");
    }

    return true;
}

typedef struct {
    PackAction action;
    Zlib *zlib;
    char **extensions;
    size_t extensionsCount;
} IpfParams;
static bool process_ipf (uint8_t *data, size_t dataSize, char *archive, char *filename, void *userdata)
{
    int status = 0;
    IpfParams *params = (IpfParams *) userdata;
    PackAction action = params->action;
    Zlib *zlib = params->zlib;
    char **extensions = params->extensions;
    size_t extensionsCount = params->extensionsCount;

    // Check if the file is encrypted
    int crypted_extension (char *filename) {
        return (
            // Those files aren't encrypted
            file_is_extension (filename, "mp3")
        ||  file_is_extension (filename, "fsb")
        ||  file_is_extension (filename, "jpg"));
    }

    // Check if the file is worth being decompressed
    int worth_decompress (char *filename, char **extensions, size_t extensionsCount) {
        for (int i = 0; i < extensionsCount; i++) {
        	if (file_is_extension (filename, extensions[i])) {
        		return true;
        	}
        }

        return false;
    }

    switch (action) 
    {
        case ACTION_EXTRACT: {
            uint8_t *fileContent = data;
            size_t fileSize = dataSize;

            // We don't decompress all the files, only those interesting
            if (worth_decompress (filename, extensions, extensionsCount) && !(crypted_extension (filename))) {
                if (!(zlibDecompress (zlib, data, dataSize))) {
                    error ("Cannot decompress '%s'.", filename);
                    goto cleanup;
                }

                fileContent = zlib->buffer;
                fileSize = zlib->header.size;
            }

            // Get basename and dirname
            char *name = basename (filename);
            char *path = dirname (filename);
            name = (name) ? name : filename;

            if (!(path && name)) {
                error ("Cannot extract directory / filename : %s / %s.", archive, filename);
                goto cleanup;
            }

            char targetPath[PATH_MAX] = {0};
            char targetFullName[PATH_MAX] = {0};
            sprintf (targetPath, "./extract/%s/%s", archive, path);
            mkpath (targetPath);
            sprintf (targetFullName, "%s/%s", targetPath, name);

            // If we decompressed it, write the data in the file
            if (worth_decompress (name, extensions, extensionsCount))
            {
                if (file_is_extension (name, "ies")) {
                    // IES parser
                    FILE *ies = fopen (targetFullName, "wb+");
                    IesParams iesParams = {.output = ies};
                    ies_read (fileContent, fileSize, process_ies, &iesParams);
                    fclose (ies);
                }
                else {
                    if (!(file_write (targetFullName, fileContent, fileSize))) {
                        error ("Cannot write data to '%s'.", targetFullName);
                        goto cleanup;
                    }
                }
            } 
            else {
                // Write the MD5 inside the file
                char md5[33] = {0};
                MD5_bufferEx (data, dataSize, md5);
                if (!(file_write (targetFullName, (uint8_t *) md5, sizeof(md5) - 1))) {
                    error ("Cannot write md5 to '%s'.", targetFullName);
                    goto cleanup;
                }
            }
        }
        break;

        case ACTION_DECRYPT:
            if (!(crypted_extension (filename))) {
                ipf_decrypt (data, dataSize);
            }
        break;

        case ACTION_ENCRYPT:
            if (!(crypted_extension (filename))) {
                ipf_encrypt (data, dataSize);
            }
        break;
    }

    status = true;
cleanup:
    return status;
}

int main (int argc, char **argv)
{
    if (argc < 3) {
        info ("Usage : ipf_decrypt.exe <crypted IPF> <encrypt|decrypt|extract> [extensions worth decompressing...]");
        return 1;
    }

    // Initialize params
    IpfParams params = {0};

    if (strcmp (argv[2], "encrypt") == 0) {
        params.action = ACTION_ENCRYPT;
    }
    else if (strcmp (argv[2], "decrypt") == 0) {
        params.action = ACTION_DECRYPT;
    }
    else if (strcmp (argv[2], "extract") == 0) {
        params.action = ACTION_EXTRACT;
        if (!(params.zlib = calloc (1, sizeof(Zlib)))) {
            error ("Cannot allocate zlib.");
            return 1;
        }
        mkpath ("./extract");
    }
    else {
        error ("Unknown action '%s'", argv[2]);
        return 1;
    }
    if (argc == 3) {
    	// Default parameters
    	params.extensions = default_decompressed_extensions;
    	params.extensionsCount = sizeof(default_decompressed_extensions) / sizeof(*default_decompressed_extensions);
    } else {
    	params.extensions = &argv[3];
    	params.extensionsCount = argc - 3;
    }

    // Read the ipf_encrypted IPF
    size_t size;
    uint8_t *ipf;

    if (!(ipf = file_map (argv[1], &size))) {
        error ("Cannot map the ipf '%s'", argv[1]);
        return 1;
    }

    // Parsing IPF
    info ("Parsing IPF '%s' (%s) ...", argv[1], argv[2]);
    if (!(ipf_read (ipf, size, process_ipf, &params))) {
        error ("Cannot read and ipf_decrypt the file '%s'", argv[1]);
        return 1;
    }

    if (!(file_flush (argv[1], ipf, size))) {
        error ("Cannot flush the file.");
        return 1;
    }

    // Cleaning
    free (params.zlib);
    info ("Done!");

    return 0;
}
