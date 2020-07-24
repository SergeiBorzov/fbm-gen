#pragma once

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <GL/glew.h>
#define GLEW_STATIC

#define DEBUG

#ifdef DEBUG
#define GLCALL(x)           \
    do {                    \
        GLClearError();     \
        x;                  \
        assert(GLLogCall());\
    } while(0)
#else
    #define GLCALL(x) x;
#endif

static inline void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static inline bool GLLogCall() {
    while(GLenum error = glGetError()) {
        fprintf(stderr, "[OpenGL_Error] (%d)\n", error);
        return false;
    }
    return true;
}