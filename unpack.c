#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

unsigned int CRC32_m_tab[] = {
  0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
  0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
  0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
  0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
  0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
  0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
  0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
  0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
  0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
  0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
  0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
  0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
  0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
  0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
  0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
  0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
  0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
  0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
  0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
  0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
  0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
  0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
  0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
  0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
  0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
  0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
  0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
  0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
  0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
  0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
  0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
  0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
  0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
  0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
  0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
  0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
  0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
  0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
  0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
  0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
  0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
  0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
  0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
  0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
  0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
  0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
  0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
  0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
  0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
  0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
  0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
  0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
  0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
  0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
  0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
  0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
  0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
  0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
  0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
  0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
  0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
  0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
  0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
  0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

/**
 * @brief Dump a buffer in the standard output
 * @param buffer An allocated buffer to dump
 * @param bufferSize The buffer size
 * @param prefix A string printed before each line of the dump
 * @return
 */

FILE *_output = NULL;

void
buffer_print (
      void *buffer,
      int bufferSize,
      char *prefix
) {
    if (!_output) {
      _output = stdout;
    }

    int curPos = 0;

    fprintf (_output, "%s ===== buffer size = %d (0x%x) ================\n", prefix, bufferSize, bufferSize);
    while (curPos < bufferSize) {
        int offset;
        fprintf (_output, "[0x%.4x] %s", curPos, prefix);
        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            fprintf (_output, " %02X", ((unsigned char *) buffer)[curPos]);
        }
        if (offset != 16) {
            for (int j = 0; j < 16 - offset; j++) {
                fprintf (_output, "   ");
            }
        }

        fprintf (_output, " | ");
        curPos -= offset;

        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            unsigned char c = ((unsigned char *) buffer)[curPos];
            fprintf (_output, "%c", isprint(c) ? c : '.');
        }

        fprintf (_output, "\n");
    }
    fprintf (_output, "%s=================================================\n", prefix);
}


#define uint8_tn(x, n)   (*((uint8_t*)&(x)+n))
#define uint8_t3(x)   uint8_tn(x,  3)

uint32_t rc_crc32(uint32_t crc, char b)
{
  return CRC32_m_tab[(crc ^ b) & 0xFF] ^ (crc >> 8);
}

void UpdateKeys(unsigned int *keys, char b)
{
  keys[1] = rc_crc32 (keys[1], b);
  keys[2] = 0x8088405 * ((uint8_t) keys[1] + keys[2]) + 1;
  keys[3] = rc_crc32 (keys[3], uint8_t3(keys[2]));
}

void Decrypt(unsigned int *keys, unsigned char *buffer, int size)
{
  if (keys[0])
  {
    size = ((unsigned int)(size - 1) >> 1) + 1;
    for (int i = 0; i < size; i++, buffer += 2) 
    {
      uint16_t v = (*((uint16_t *)keys + 6) & 0xFFFD) | 2;
      *buffer ^= (v * (v ^ 1)) >> 8;

      keys[2] = 0x8088405 * (keys[2] + (uint8_t)((*((uint16_t *)keys + 2) >> 8) ^ LOBYTE(CRC32_m_tab[(uint8_t)(keys[1] ^ *buffer)]))) + 1;
      keys[1] = rc_crc32 (keys[1], *buffer);
      keys[3] = rc_crc32 (keys[3], uint8_t3(keys[2]));
    }
  }
}

void GenerateKeys(unsigned int *keys) 
{
    unsigned char password[20] = {0x6F, 0x66, 0x4F, 0x31, 0x61, 0x30, 0x75, 0x65, 0x58, 0x41, 0x3F, 0x20, 0x5B, 0xFF, 0x73, 0x20, 0x68, 0x20, 0x25, 0x3F};

    keys[1] = 0x12345678;
    keys[2] = 0x23456789;
    keys[3] = 0x34567890;

    for (int i = 0; i < sizeof(password); i++) {
      UpdateKeys(keys, password[i]);
    }
}

void Encrypt(unsigned int *keys, uint8_t *buffer, int size)
{
  unsigned int v4;
  uint32_t v5;
  unsigned int v6;
  int counter;

  if (keys[0] && size)
  {
    counter = ((unsigned int)(size - 1) >> 1) + 1;
    do
    {
      v4 = keys[3];
      v5 = (keys[1] >> 8) ^ CRC32_m_tab[(unsigned __int8)(keys[1] ^ *buffer)];
      keys[1] = v5;
      v6 = 0x8088405 * (keys[2] + (unsigned __int8)v5) + 1;
      keys[2] = v6;
      keys[3] = CRC32_m_tab[(unsigned __int8)(v4 ^ uint8_t3(v6))] ^ (v4 >> 8);
      *buffer ^= (unsigned __int16)(((v4 & 0xFFFD) | 2) * (((v4 & 0xFFFD) | 2) ^ 1)) >> 8;
      buffer += 2;
      --counter;
    } while (counter);
  }
}

size_t file_get_size (const char *filename) 
{
  FILE *f;

  if (!(f = fopen (filename, "rb"))) {
    fprintf (stderr, "%s cannot be opened\n", filename);
    return 0;
  }

  fseek (f, 0, SEEK_END);
  size_t size = ftell (f);
  fclose(f);

  return size;
}

unsigned char *file_map (const char *filename, size_t *_size) 
{
  HANDLE hIpf, hMap;
  if (!(hIpf = CreateFile (filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL))) {
    fprintf (stderr, "Cannot CreateFile '%s'\n", filename);
    return NULL;
  }

  if (!(hMap = CreateFileMapping (hIpf, NULL, PAGE_READWRITE, 0, 0, NULL))) {
    fprintf (stderr, "Cannot create file mapping '%s'\n", filename);
    return NULL;
  }

  size_t fileSize = GetFileSize (hIpf, NULL);
  void *map = NULL;
  if (!(map = MapViewOfFile (hMap, FILE_MAP_ALL_ACCESS, 0, 0, fileSize))) {
    fprintf (stderr, "Cannot map file '%s'\n", filename);
    return NULL;
  }

  *_size = fileSize;
  return map;
}

int file_write (const char *filename, unsigned char *buffer, size_t size) 
{
  FILE *f = NULL;

  if (!(f = fopen (filename, "wb+"))) {
    printf ("%s cannot be opened\n", filename);
    return 0;
  }

  fwrite (buffer, size, 1, f);
  fclose (f);

  return 1;
}

int is_extension (char *filename, char *extension)
{
    const char *dot = strrchr (filename, '.');

    if (!dot || dot == filename) {
      // no extension
      return 0;
    }

    return strcmp (dot + 1, extension) == 0;
}

int read_ipf (unsigned char *ipf, size_t size) 
{
  unsigned char *header = &ipf[size-24];

  #pragma pack(push, 1)
  struct ArchiveHeader {
      uint16_t file_count;
      uint32_t filetable_offset;
      uint16_t unk;
      uint32_t filefooter_offset;
      uint32_t magic;
      uint32_t base_revision;
      uint32_t revision;
  } *archive_header = (void *) header;
  #pragma pack(pop)

  char magic[] = {0x50, 0x4b, 0x05, 0x06};

  if (memcmp (&archive_header->magic, magic, sizeof(magic)) != 0) {
    fprintf (stderr, "Wrong magic word : %#x\n", archive_header->magic);
    return 0;
  }

  #pragma pack(push, 1)
  struct IpfInfo {
      uint16_t filename_length;
      uint32_t crc;
      uint32_t compressed_length;
      uint32_t uncompressed_length;
      uint32_t data_offset;
      uint16_t archivename_length;
  } *ipf_info = (void *) &ipf[archive_header->filetable_offset];
  #pragma pack(pop)

  char *cursor = (void *) ipf_info;

  for (int i = 0; i < archive_header->file_count; i++, ipf_info = (void *) cursor) {
    // Iterate through all the files
    // Get the file data
    unsigned char *data = &ipf[ipf_info->data_offset];
    size_t dataSize = ipf_info->compressed_length;

    unsigned int keys[4] = {1, 0, 0, 0};
    GenerateKeys(keys);

    char *archive_ptr = cursor + sizeof (*ipf_info);
    char archive[ipf_info->archivename_length + 1];
    memset (archive, 0, sizeof(archive));
    strncpy (archive, archive_ptr, ipf_info->archivename_length);

    char *filename_ptr = archive_ptr + ipf_info->archivename_length;
    char filename[ipf_info->filename_length + 1];
    memset (filename, 0, sizeof(filename));
    strncpy (filename, filename_ptr, ipf_info->filename_length);

    if (!(
       is_extension (filename, "mp3")
    || is_extension (filename, "fsb")
    || is_extension (filename, "jpg")
    || is_extension (filename, "JPG"))
    ) {
      // Those files aren't encrypted
      Decrypt(keys, data, dataSize);
    }

    cursor += sizeof (*ipf_info);
    cursor += ipf_info->archivename_length;
    cursor += ipf_info->filename_length;
  }

  return 1;
}

int main (int argc, char **argv) {

    if (argc != 2) {
      printf ("Usage : decrypt.exe <crypted IPF>\n");
      return 0;
    }

    // Read the encrypted IPF
    size_t size;
    unsigned char *ipf;
    if (!(ipf = file_map (argv[1], &size))) {
      fprintf (stderr, "[!] Cannot map the ipf '%s'\n", argv[1]);
      return -1;
    }
    
    // Parsing IPF
    fprintf (stderr, "[+] Parsing IPF '%s' ... ", argv[1]);
    if (!(read_ipf (ipf, size))) {
      fprintf (stderr, "\n[!] Cannot read and decrypt the file '%s'\n", argv[1]);
    }

    // Unmap
    UnmapViewOfFile (ipf);
    fprintf (stderr, "Done!\n");

    return 0;
}