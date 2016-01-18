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

#include "fs.h"
#include "dbg/dbg.h"
#include <limits.h>

#ifdef WIN32
// File mapping for Win32
uint8_t *file_map (char *filename, size_t *_size) 
{
    HANDLE hIpf, hMap;
    if (!(hIpf = CreateFile (filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL))) {
        error ("Cannot CreateFile '%s'. Reason : %d.", filename, (int) GetLastError ());
        return NULL;
    }

    if (!(hMap = CreateFileMapping (hIpf, NULL, PAGE_READWRITE, 0, 0, NULL))) {
        error ("Cannot create file mapping '%s'. Reason : %d.", filename, (int) GetLastError ());
        return NULL;
    }

    size_t fileSize;
    if ((fileSize = GetFileSize (hIpf, NULL)) == INVALID_FILE_SIZE) {
        printf ("Cannot get '%s' file size correctly. Reason : %d.\n", filename, (int) GetLastError ());
        return NULL;
    }

    void *map = NULL;
    if (!(map = MapViewOfFile (hMap, FILE_MAP_ALL_ACCESS, 0, 0, fileSize))) {
        error ("Cannot map file '%s'. Reason : %d.", filename, (int) GetLastError ());
        return NULL;
    }

    *_size = fileSize;
    return map;
}

int file_flush (char *filename, void *data, size_t size)
{
    // Flush and Unmap
    if (!(FlushViewOfFile (data, size))) {
        error ("Cannot flush the file. Reason : %d.", (int) GetLastError ());
        return 0;
    }

    if (!(UnmapViewOfFile (data))) {
        error ("Cannot unmap the view of file. Reason : %d.", (int) GetLastError ());
        return 0;
    }

    return 1;
}
#else
// File mapping for Linux
uint8_t *file_map (char *filename, size_t *_size) 
{
    struct stat sbuf;
    if (stat(filename, &sbuf) != 0) {
	error ("Cannot get file size.");
	return NULL;
    }
    size_t fileSize = sbuf.st_size;

    int hIpf;
    if (!(hIpf = open (filename, O_RDWR))) {
        error ("Cannot open '%s'. Reason : %s.", filename, strerror (errno));
        return NULL;
    }

    void *map = NULL;
    info ("FileSize = %d", fileSize);
    if ((map = mmap (NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, hIpf, 0)) == MAP_FAILED) {
        error ("Cannot map file '%s'. Reason : %s.", filename, strerror (errno));
        return NULL;
    }

    *_size = fileSize;
    return map;
}

int file_flush (char *filename, void *data, size_t size)
{
    if (!(file_write (filename, data, size))) {
        error ("Cannot write data to file '%s'.", filename);
        return 0;
    }
    
    if (munmap (data, size) != 0) {
        error ("Cannot unmap file.");
        return 0;
    }

    return 1;
}
#endif

int file_write (char *filename, uint8_t *buffer, size_t size) 
{
    return 1;
    FILE *f = NULL;

    if (!(f = fopen (filename, "wb+"))) {
        error ("%s cannot be opened", filename);
        return 0;
    }

    if (size != 0 && fwrite (buffer, size, 1, f) != 1) {
        error ("Cannot write '%llu' bytes to '%s'\n.", size, filename);
        return 0;
    }

    if (fclose (f) != 0) {
        error ("Cannot close successfully the file '%s'.", filename);
        return 0;
    }

    return 1;
}

int file_is_extension (char *filename, char *extension)
{
    char *dot = strrchr (filename, '.');

    if (!dot || dot == filename) {
        // no extension
        return 0;
    }

    return strcasecmp (dot + 1, extension) == 0;
}

static int _mkdir (char *path) {
    int status;

    #if defined(WIN32) || defined(__CYGWIN__)
    status = CreateDirectory (path, NULL);
    #else
    status = mkdir (path, S_IRWXU);
    #endif

    return status;
}

static void recursive_mkdir (const char *dir)
{
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    strncpy (tmp, dir, sizeof(tmp));
    len = strlen (tmp);

    if (tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            _mkdir (tmp);
            *p = '/';
        }
        _mkdir (tmp);
    }
}

void mkpath (char *path) {
    recursive_mkdir (path);
}
