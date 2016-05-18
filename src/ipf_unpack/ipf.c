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

#include "ipf.h"
#include "dbg/dbg.h"

#pragma pack (push, 1)
typedef struct {
    uint16_t fileCount;
    uint32_t filetableOffset;
    uint16_t unk;
    uint32_t filefooterOffset;
    uint32_t magic;
    uint32_t baseRevision;
    uint32_t revision;
} ArchiveHeader;
#pragma pack (pop)

#pragma pack (push, 1)
typedef struct {
    uint16_t filenameLength;
    uint32_t crc;
    uint32_t compressedLength;
    uint32_t uncompressedLength;
    uint32_t dataOffset;
    uint16_t archivenameLength;
} IpfInfo;
#pragma pack (pop)

bool ipf_read (uint8_t *ipf, size_t size, IpfCallback callback, void *userdata) 
{
    uint8_t *header = &ipf[size-24];
    bool status = false;

    ArchiveHeader *archiveHeader = (void *) header;
    char magic[] = {0x50, 0x4b, 0x05, 0x06};

    if (memcmp (&archiveHeader->magic, magic, sizeof(magic)) != 0) {
        error ("Wrong magic word : %#x", archiveHeader->magic);
        goto cleanup;
    }

    IpfInfo *ipfInfo = (void *) &ipf[archiveHeader->filetableOffset];
    char *cursor = (void *) ipfInfo;
    
    // Iterate through all the files
    for (int i = 0; i < archiveHeader->fileCount; i++, ipfInfo = (void *) cursor)
    {
        // Get the current file data
        uint8_t *data = &ipf[ipfInfo->dataOffset];
        size_t dataSize = ipfInfo->compressedLength;

        char *archive_ptr = cursor + sizeof(*ipfInfo);
        char archive[ipfInfo->archivenameLength + 1];
        memset (archive, 0, sizeof(archive));
        strncpy (archive, archive_ptr, ipfInfo->archivenameLength);

        char *filename_ptr = archive_ptr + ipfInfo->archivenameLength;
        char filename[ipfInfo->filenameLength + 1];
        memset (filename, 0, sizeof(filename));
        strncpy (filename, filename_ptr, ipfInfo->filenameLength);

        if (!(callback (data, dataSize, archive, filename, userdata))) {
            error ("callback failed for '%s:%s'", archive, filename);
        }

        cursor += sizeof(*ipfInfo);
        cursor += ipfInfo->archivenameLength;
        cursor += ipfInfo->filenameLength;
    }

    status = true;
cleanup:
    return status;
}
