#ifndef FBMGEN_UTILS
#define FBMGEN_UTILS

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


namespace fbmgen {
    bool ReadFileToString(const char* filename, char** str, size_t* size);
    bool GetAbsolutePath(char* buffer, size_t buffer_size);
}


#endif /* End of FBMGEN_UTILS */