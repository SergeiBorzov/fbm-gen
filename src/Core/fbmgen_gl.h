#pragma once

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <GL/glew.h>
#define GLEW_STATIC

#define DEBUG_GL

#ifdef DEBUG_GL
#define GLCall(x) GLClearError();\
    x;\
    assert(GLLogCall());
#else
    #define GLCall(x) x;
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