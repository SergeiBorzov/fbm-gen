#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../Core/fbmgen_gl.h"
#include "Texture.h"

namespace fbmgen {

    Texture* Texture::CreateEmpty(s32 width, s32 height, s32 channels) {
        Texture* result = new Texture();
        GLCall(glGenTextures(1, &result->m_Handle);)
        result->Bind();

        switch(channels) {
            case 1: {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);)
                break;
            }
            case 2: {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);)
                break;
            }
            case 3: {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);)
                break;
            }
            case 4: {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);)
                break;
            }
            default: {
                assert(false);
                return nullptr;
            }
        }
        GLCall(glGenerateMipmap(GL_TEXTURE_2D);)

        result->SetFilterMode(result->m_FilterMode);
        result->SetWrapMode(result->m_WrapMode);

        return result;
    }

    Texture* Texture::Load(const char* filename) {
        Texture* result = new Texture();
        GLCall(glGenTextures(1, &result->m_Handle);)
        result->Bind();

        unsigned char *data = stbi_load(filename, &result->m_Width, &result->m_Height, &result->m_Channels, 0);

        if (data == nullptr) {
            fprintf(stderr, "Failed to load texture %s\n", filename);
            delete result;
            return nullptr;
        }

        switch(result->m_Channels) {
            case 1: {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, result->m_Width, result->m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, data);)
                break;
            }
            case 2: {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, result->m_Width, result->m_Height, 0, GL_RG, GL_UNSIGNED_BYTE, data);)
                break;
            }
            case 3: {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, result->m_Width, result->m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);)
                break;
            }
            case 4: {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result->m_Width, result->m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);)
                break;
            }
            default: {
                assert(false);
                return nullptr;
            }
        }
        GLCall(glGenerateMipmap(GL_TEXTURE_2D);)

        result->SetFilterMode(result->m_FilterMode);
        result->SetWrapMode(result->m_WrapMode);

        // Here data is not NULL anymore
        stbi_image_free(data);
        return result;
    }

    void Texture::SetWrapMode(WrapMode mode) {
        Bind();
        m_WrapMode = mode;
        switch(m_WrapMode) {
            case WrapMode::Repeat: {
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);)	
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);)
                break;
            }
            case WrapMode::Mirrored: {
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);)	
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);)
                break;
            }
            case WrapMode::Edge: {
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);)	
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);)
                break;
            }
            case WrapMode::Border: {
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);)	
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);)
                break;
            }
        }
    }

    void Texture::SetFilterMode(FilterMode mode) {
        Bind();
        m_FilterMode = mode;
        switch(m_FilterMode) {
            case FilterMode::Nearest: {
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);)
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);)
                break;
            }
            case FilterMode::Linear: {
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);)
                GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);)
                break;
            }
        }
    }

    void Texture::Bind() const {
        GLCall(glBindTexture(GL_TEXTURE_2D, m_Handle);)
    }

    Texture::~Texture() {
        if (m_Handle) {
            GLCall(glDeleteTextures(1, &m_Handle);)
        }
    }
}