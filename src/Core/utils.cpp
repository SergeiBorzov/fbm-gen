#include <cstdio>
#include <cstdlib>

#include "types.h"
#include "utils.h"

namespace fbmgen {
    bool ReadFileToString(const char* filename, char** buffer, size_t* buffer_size) {
        FILE *f;
        size_t file_size;
        f = fopen (filename, "rb");
        if(!f) {
            fprintf(stderr, "Failed to open file %s\n", filename);
            return false;
        }

        if (fseek( f , 0L , SEEK_END) != 0) {
            fclose(f);
            fprintf(stderr, "Error during file reading %s\n", filename);
            return false;
        }

        file_size = ftell(f);
        *buffer_size = file_size + 1;
        if (file_size == -1) {
            fclose(f);
            fprintf(stderr, "Error during file reading %s\n", filename);
            return false;
        }
        rewind(f);

        /* allocate memory for entire content */
        *buffer = static_cast<char*>(calloc(1, *buffer_size));
        if (!*buffer) {
            fclose(f);
            fprintf(stderr, "Failed to allocate memory for file %s\n", filename);
            return false;
        }

        /* copy the file into the buffer */
        if (fread(*buffer , file_size, 1 , f) != 1) {
            fclose(f);
            free(*buffer);
            fprintf(stderr, "Failed to copy content of file %s into buffer\n", filename);
        }
        fclose(f);
        return true;
    }

    bool GetAbsolutePath(char* buffer, size_t buffer_size) {
#ifdef _WIN32
        char path_exe[MAX_PATH];
        GetModuleFileNameA(NULL, path_exe, MAX_PATH);
        char *last = strrchr(path_exe, '\\');
        
        if (last) {
            size_t len = (size_t) (last - path_exe + 1);
            if (len + 1 > buffer_size) {
                fprintf(stderr, "Buffer is too small for absolute path\n");
                return false;
            }
            strncpy(buffer, path_exe, len);
            buffer[len] = '\0';
        }
        return true;
#else
        return false;
#endif
    }
}