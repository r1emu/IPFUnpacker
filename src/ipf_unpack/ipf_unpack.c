#include "ipf_unpack.h"
#include "dbg/dbg.h"
#include "crc32/crc32.h"
#include "fs/fs.h"

void generate_keys (uint32_t *keys) 
{
    uint8_t password[20] = {0x6F, 0x66, 0x4F, 0x31, 0x61, 0x30, 0x75, 0x65, 0x58, 0x41, 0x3F, 0x20, 0x5B, 0xFF, 0x73, 0x20, 0x68, 0x20, 0x25, 0x3F};

    keys[1] = 0x12345678;
    keys[2] = 0x23456789;
    keys[3] = 0x34567890;

    for (int i = 0; i < sizeof(password); i++) {
        update_keys (keys, password[i]);
    }
}

void update_keys (uint32_t *keys, char b)
{
    keys[1] = crc32 (keys[1], b);
    keys[2] = 0x8088405 * ((uint8_t) keys[1] + keys[2]) + 1;
    keys[3] = crc32 (keys[3], BYTE3 (keys[2]));
}

void decrypt (uint32_t *keys, uint8_t *buffer, size_t size)
{
    if (keys[0]) {
        size = ((size - 1) >> 1) + 1;
        for (int i = 0; i < size; i++, buffer += 2) {
            uint16_t v = (* ((uint16_t *)keys + 6) & 0xFFFD) | 2;
            *buffer ^= (v * (v ^ 1)) >> 8;

            keys[2] = 0x8088405 * (keys[2] + (uint8_t) ((* ((uint16_t *)keys + 2) >> 8) ^ LOBYTE (CRC32_m_tab[ (uint8_t) (keys[1] ^ *buffer)]))) + 1;
            keys[1] = crc32 (keys[1], *buffer);
            keys[3] = crc32 (keys[3], BYTE3 (keys[2]));
        }
    }
}

void encrypt (uint32_t *keys, uint8_t *buffer, uint32_t size)
{
    uint32_t v4;
    uint32_t v5;
    uint32_t v6;
    int counter;

    if (keys[0] && size)
    {
        counter = ((size - 1) >> 1) + 1;
        do {
            v4 = keys[3];
            v5 = (keys[1] >> 8) ^ CRC32_m_tab[ (uint8_t) (keys[1] ^ *buffer)];
            keys[1] = v5;
            v6 = 0x8088405 * (keys[2] + (uint8_t)v5) + 1;
            keys[2] = v6;
            keys[3] = CRC32_m_tab[ (uint8_t) (v4 ^ BYTE3 (v6))] ^ (v4 >> 8);
            *buffer ^= (uint16_t) (( (v4 & 0xFFFD) | 2) * (( (v4 & 0xFFFD) | 2) ^ 1)) >> 8;
            buffer += 2;
            --counter;
        } while (counter);
    }
}

int read_ipf (uint8_t *ipf, size_t size, PackAction action) 
{
    uint8_t *header = &ipf[size-24];

    #pragma pack (push, 1)
    struct ArchiveHeader {
        uint16_t file_count;
        uint32_t filetable_offset;
        uint16_t unk;
        uint32_t filefooter_offset;
        uint32_t magic;
        uint32_t base_revision;
        uint32_t revision;
    } *archive_header = (void *) header;
    #pragma pack (pop)

    char magic[] = {0x50, 0x4b, 0x05, 0x06};

    if (memcmp (&archive_header->magic, magic, sizeof(magic)) != 0) {
        error ("Wrong magic word : %#x", archive_header->magic);
        return 0;
    }

    #pragma pack (push, 1)
    struct IpfInfo {
        uint16_t filename_length;
        uint32_t crc;
        uint32_t compressed_length;
        uint32_t uncompressed_length;
        uint32_t data_offset;
        uint16_t archivename_length;
    } *ipf_info = (void *) &ipf[archive_header->filetable_offset];
    #pragma pack (pop)

    char *cursor = (void *) ipf_info;

    for (int i = 0; i < archive_header->file_count; i++, ipf_info = (void *) cursor) {
        // Iterate through all the files
        // Get the file data
        uint8_t *data = &ipf[ipf_info->data_offset];
        size_t dataSize = ipf_info->compressed_length;

        uint32_t keys[4] = {1, 0, 0, 0};
        generate_keys (keys);

        char *archive_ptr = cursor + sizeof(*ipf_info);
        char archive[ipf_info->archivename_length + 1];
        memset (archive, 0, sizeof(archive));
        strncpy (archive, archive_ptr, ipf_info->archivename_length);

        char *filename_ptr = archive_ptr + ipf_info->archivename_length;
        char filename[ipf_info->filename_length + 1];
        memset (filename, 0, sizeof(filename));
        strncpy (filename, filename_ptr, ipf_info->filename_length);

        if (!(
            // Those files aren't encrypted
           file_is_extension (filename, "mp3")
        || file_is_extension (filename, "fsb")
        || file_is_extension (filename, "jpg")
        || file_is_extension (filename, "JPG"))
        ) {
            switch (action) {
                case ACTION_EXTRACT:
                    decrypt (keys, data, dataSize);
                break;

                case ACTION_DECRYPT:
                    decrypt (keys, data, dataSize);
                break;

                case ACTION_ENCRYPT:
                    encrypt (keys, data, dataSize);
                break;
            }
        }

        cursor += sizeof(*ipf_info);
        cursor += ipf_info->archivename_length;
        cursor += ipf_info->filename_length;
    }

    return 1;
}

int main (int argc, char **argv)
{
    if (argc != 3) {
        info ("Usage : decrypt.exe <crypted IPF> [encrypt/decrypt]");
        return 0;
    }

    PackAction action;

    if (strcmp (argv[2], "encrypt") == 0) {
        action = ACTION_ENCRYPT;
    }
    else if (strcmp (argv[2], "decrypt") == 0) {
        action = ACTION_DECRYPT;
    }
    else if (strcmp (argv[2], "extract") == 0) {
        action = ACTION_EXTRACT;
    }
    else {
        error ("Unknown action '%s'", argv[2]);
        return -1;
    }

    // Read the encrypted IPF
    size_t size;
    uint8_t *ipf;

    if (!(ipf = file_map (argv[1], &size))) {
        error ("Cannot map the ipf '%s'", argv[1]);
        return -1;
    }

    // Parsing IPF
    info ("Parsing IPF '%s' (%s) ...", argv[1], argv[2]);
    if (!(read_ipf (ipf, size, action))) {
        error ("Cannot read and decrypt the file '%s'", argv[1]);
        return 0;
    }

    if (!(file_flush (argv[1], ipf, size))) {
        error ("Cannot flush the file.");
        return 0;
    }

    info ("Done!");

    return 0;
}