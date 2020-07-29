#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../Core/fbmgen_gl.h"
#include "Texture.h"

namespace fbmgen {

    Texture* Texture::CreateEmpty(s32 width, s32 height, s32 channels) {
        Texture* result = new Texture();
        GLCALL(glGenTextures(1, &result->m_Handle));
        result->Bind();
        result->m_Width = width;
        result->m_Height = height;
        result->m_Channels = channels;

        switch(channels) {
            case 1: {
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL));
                break;
            }
            case 2: {
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, NULL));
                break;
            }
            case 3: {
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
                break;
            }
            case 4: {
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
                break;
            }
            default: {
                assert(false);
                return nullptr;
            }
        }
        GLCALL(glGenerateMipmap(GL_TEXTURE_2D));

        result->SetFilterMode(result->m_FilterMode);
        result->SetWrapMode(result->m_WrapMode);

        return result;
    }

    Texture* Texture::Load(const char* filename) {
        Texture* result = new Texture();
        GLCALL(glGenTextures(1, &result->m_Handle));
        result->Bind();

        unsigned char *data = stbi_load(filename, &result->m_Width, &result->m_Height, &result->m_Channels, 0);

        if (data == nullptr) {
            fprintf(stderr, "Failed to load texture %s\n", filename);
            delete result;
            return nullptr;
        }

        switch(result->m_Channels) {
            case 1: {
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, result->m_Width, result->m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, data));
                break;
            }
            case 2: {
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, result->m_Width, result->m_Height, 0, GL_RG, GL_UNSIGNED_BYTE, data));
                break;
            }
            case 3: {
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, result->m_Width, result->m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
                break;
            }
            case 4: {
                GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result->m_Width, result->m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
                break;
            }
            default: {
                assert(false);
                return nullptr;
            }
        }
        GLCALL(glGenerateMipmap(GL_TEXTURE_2D));

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
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
                break;
            }
            case WrapMode::Mirrored: {
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));	
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
                break;
            }
            case WrapMode::Edge: {
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));	
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
                break;
            }
            case WrapMode::Border: {
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));	
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
                break;
            }
        }
    }

    void Texture::SetFilterMode(FilterMode mode) {
        Bind();
        m_FilterMode = mode;
        switch(m_FilterMode) {
            case FilterMode::Nearest: {
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
                break;
            }
            case FilterMode::Linear: {
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
                GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
                break;
            }
        }
    }

    void Texture::Bind() const {
        GLCALL(glBindTexture(GL_TEXTURE_2D, m_Handle));
    }

    void Texture::Resize(s32 width, s32 height) {
        Bind();
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
        m_Width = width;
        m_Height = height;
    }

    Texture::~Texture() {
        if (m_Handle) {
            GLCALL(glDeleteTextures(1, &m_Handle));
        }
    }
}