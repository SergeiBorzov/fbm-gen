#include <string>

#include <glm/gtc/type_ptr.hpp>

#include "../Core/utils.h"
#include "Shader.h"

namespace fbmgen {
   
    Shader* Shader::Load(const char* filename) {
        Shader* result = new Shader();

        // Read shader file to shaderData
        char* shaderData;
        size_t file_size;

        if (!ReadFileToString(filename, &shaderData, &file_size)) {
            delete result;
            return nullptr;
        }

        bool hasVertex = false;
        bool hasFragment = false;

        std::string shader_source = shaderData;

        size_t v_index = shader_source.find("__VERTEX__");
        if (v_index == std::string::npos) {
            delete result;
            free(shaderData);
            return nullptr;
        }
        hasVertex = true;

        size_t f_index = shader_source.find("__FRAGMENT__");
        if (f_index == std::string::npos) {
            delete result;
            free(shaderData);
            return nullptr;
        }
        hasFragment = true;

        if (!hasVertex || !hasFragment) {
            delete result;
            free(shaderData);
            return nullptr;
        }

        result->m_ShaderMap[GL_VERTEX_SHADER] = shader_source.substr(v_index + strlen("__VERTEX__"), f_index - v_index - strlen("__VERTEX__"));
        result->m_ShaderMap[GL_FRAGMENT_SHADER]  = shader_source.substr(f_index + strlen("__FRAGMENT__"));

        if (!result->Compile()) {
            delete result;
            free(shaderData);
            return nullptr;
        }
        
        // Shader is ready to use
        free(shaderData);
        return result;
    }

    bool Shader::CompileSuccess(u32 handle) {
        int success;
        char infoLog[512];
        GLCALL(glGetShaderiv(handle, GL_COMPILE_STATUS, &success));

        if(!success) {
            GLCALL(glGetShaderInfoLog(handle, 512, NULL, infoLog));
            fprintf(stderr, "Shader compilation failed: %s\n", infoLog);
        }
        return success;
    }

    bool Shader::CompilePart(const char* source, GLenum type) {
        GLCALL(m_ShaderHandles[type] = glCreateShader(type));
        GLCALL(glShaderSource(m_ShaderHandles[type], 1, &source, NULL));
        GLCALL(glCompileShader(m_ShaderHandles[type]));
        return true;
    }


    bool Shader::LinkSuccess() {
        int success;
        char infoLog[512];
        GLCALL(glGetProgramiv(m_Handle, GL_LINK_STATUS, &success));
        if(!success) {
            GLCALL(glGetProgramInfoLog(m_Handle, 512, NULL, infoLog));
            fprintf(stderr, "Shader linking failed: %s\n", infoLog);
        }
        return success;
    }

    bool Shader::Compile() {
        GLCALL(m_Handle = glCreateProgram());

        for (const auto& p: m_ShaderMap) {
            CompilePart(p.second.c_str(), p.first);
            if (!CompileSuccess(m_ShaderHandles[p.first])) {
                for (const auto& item: m_ShaderHandles) {
                    if (item.second) {
                        GLCALL(glDeleteShader(item.second));
                    }
                }
                return false;
            }
            GLCALL(glAttachShader(m_Handle, m_ShaderHandles[p.first]));
        }

        GLCALL(glLinkProgram(m_Handle));
        if (!LinkSuccess()) {
            return false;
        }

        for (const auto& p: m_ShaderHandles) {
            GLCALL(glDeleteShader(p.second));
        }

        m_ShaderHandles.clear();
        return true;
    }

    void Shader::Run() {
        GLCALL(glUseProgram(m_Handle));
    }

    void Shader::SetUniform(const char* name, s32 value) {
        s32 location;
        GLCALL(location = glGetUniformLocation(m_Handle, name));
        assert(location != -1);
        GLCALL(glUniform1i(location, value));
    }

    void Shader::SetUniform(const char* name, s32* values, s32 count) {
        s32 location;
        GLCALL(location = glGetUniformLocation(m_Handle, name));
        assert(location != -1);
        GLCALL(glUniform1iv(location, count, values));
    }

     void Shader::SetUniform(const char* name, f32 value) {
        s32 location;
        GLCALL(location = glGetUniformLocation(m_Handle, name));
        assert(location != -1);
        GLCALL(glUniform1f(location, value));
    }

    void Shader::SetUniform(const char* name, const glm::vec2& vec) {
        s32 location;
        GLCALL(location = glGetUniformLocation(m_Handle, name));
        assert(location != -1);
        GLCALL(glUniform2f(location, vec.x, vec.y));
    }

     void Shader::SetUniform(const char* name, const glm::vec3& vec) {
        s32 location;
        GLCALL(location = glGetUniformLocation(m_Handle, name));
        assert(location != -1);
        GLCALL(glUniform3f(location, vec.x, vec.y, vec.z));
    }

     void Shader::SetUniform(const char* name, const glm::vec4& vec) {
        s32 location;
        GLCALL(location = glGetUniformLocation(m_Handle, name));
        assert(location != -1);
        GLCALL(glUniform4f(location, vec.x, vec.y, vec.z, vec.w));
    }

    void Shader::SetUniform(const char* name, const glm::mat3& matrix) {
        s32 location;
        GLCALL(location = glGetUniformLocation(m_Handle, name));
        assert(location != -1);
        GLCALL(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
    }

    void Shader::SetUniform(const char* name, const glm::mat4& matrix) {
        s32 location;
        GLCALL(location = glGetUniformLocation(m_Handle, name));
        assert(location != -1);
        GLCALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
    }

    

    Shader::~Shader() {
        if (m_Handle) {
            GLCALL(glDeleteProgram(m_Handle));
        }
    }
}
