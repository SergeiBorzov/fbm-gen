#include "../Core/fbmgen_gl.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace fbmgen {

    bool Texture::Load(const char* filename) {

        GLCall(glGenTextures(1, &m_Handle));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_Handle);)
	    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);)
	    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);)

        int w, h, c;
        unsigned char *data = stbi_load(filename, &w, &h, &c, 4);
        // Creating empty texture
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);)

        stbi_image_free(data);

        m_Inited = true;
        return true;
    }

    Texture::~Texture() {
        if (m_Inited) {
            GLCall(glDeleteTextures(1, &m_Handle);)
        }
    }
    
}