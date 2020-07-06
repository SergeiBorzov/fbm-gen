#include <cjson/cJSON.h>

#include <glm/gtc/type_ptr.hpp>

#include "../Core/utils.h"
#include "Shader.h"

namespace fbmgen {
   
    Shader* Shader::Load(const char* filename) {
        Shader* result = new Shader();

        // Read shader file to shaderData
        char* shaderData;
        size_t file_size;

        if (!ReadTextFileToString(filename, &shaderData, &file_size)) {
            delete result;
            return nullptr;
        }

        cJSON *shader_json = cJSON_Parse(shaderData);
        if (shader_json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                fprintf(stderr, "Error parsing json file %s\n", error_ptr);
            }
            delete result;
            return nullptr;
        }

        // Check Vertex and Fragment Shaders
        bool hasVertex = false;
        bool hasFragment = false;

        const cJSON *vertexPath = cJSON_GetObjectItemCaseSensitive(shader_json, "Vertex");
        if (!cJSON_IsString(vertexPath) || (vertexPath->valuestring == NULL)) {
            fprintf(stderr, "Invealid vertex shader description\n");
            delete result;
            return nullptr;
        }
        result->m_ShaderMap[GL_VERTEX_SHADER] = vertexPath->valuestring;
        hasVertex = true;

        const cJSON *fragmentPath = cJSON_GetObjectItemCaseSensitive(shader_json, "Fragment");
        if (!cJSON_IsString(fragmentPath) || (fragmentPath->valuestring == NULL)) {
            fprintf(stderr, "Invalid fragment shader description\n");
            delete result;
            return nullptr;
        }
        result->m_ShaderMap[GL_FRAGMENT_SHADER] = fragmentPath->valuestring;
        hasFragment = true;

        const cJSON *geometryPath = cJSON_GetObjectItemCaseSensitive(shader_json, "Geometry");
        if (cJSON_IsString(geometryPath) && (geometryPath->valuestring != NULL)) {
            result->m_ShaderMap[GL_GEOMETRY_SHADER] = geometryPath->valuestring;
        }

        // Try to compile
        if (!result->Compile()) {
            delete result;
            return nullptr;
        }

        free(shaderData);
        cJSON_Delete(shader_json);
        
        // Shader is ready to use
        return result;
    }

    bool Shader::CompileSuccess(u32 handle) {
        int success;
        char infoLog[512];
        GLCall(glGetShaderiv(handle, GL_COMPILE_STATUS, &success);)

        if(!success) {
            GLCall(glGetShaderInfoLog(handle, 512, NULL, infoLog);)
            fprintf(stderr, "Shader compilation failed %s\n", infoLog);
        }
        return success;
    }

    bool Shader::CompilePart(const char* filename, GLenum type) {
        GLCall(m_ShaderHandles[type] = glCreateShader(type);)

        char* source = NULL;
        size_t size;
        if (!ReadTextFileToString(filename, &source, &size)) {
            return false;
        }
        GLCall(glShaderSource(m_ShaderHandles[type], 1, &source, NULL);)
        GLCall(glCompileShader(m_ShaderHandles[type]);)

        free(source);
        return true;
    }


    bool Shader::LinkSuccess() {
        int success;
        char infoLog[512];
        GLCall(glGetProgramiv(m_Handle, GL_LINK_STATUS, &success);)
        if(!success) {
            GLCall(glGetProgramInfoLog(m_Handle, 512, NULL, infoLog);)
            fprintf(stderr, "Shader linking failed: %s\n", infoLog);
        }
        return success;
    }

    bool Shader::Compile() {
        GLCall(m_Handle = glCreateProgram();)

        for (const auto& p: m_ShaderMap) {
            CompilePart(p.second, p.first);
            if (!CompileSuccess(m_ShaderHandles[p.first])) {
                for (const auto& item: m_ShaderHandles) {
                    if (item.second) {
                        GLCall(glDeleteShader(item.second);)
                    }
                }
                return false;
            }
            GLCall(glAttachShader(m_Handle, m_ShaderHandles[p.first]);)
        }

        GLCall(glLinkProgram(m_Handle);)
        if (!LinkSuccess()) {
            return false;
        }

        for (const auto& p: m_ShaderHandles) {
            GLCall(glDeleteShader(p.second);)
        }

        m_ShaderHandles.clear();
        return true;
    }

    void Shader::Run() {
        GLCall(glUseProgram(m_Handle);)
    }

    /*void Shader::SetUniform(const std::string& name, s32 value) {
        GLCall(s32 location = glGetUniformLocation(m_Handle, name.c_str());)
        assert(location != -1);
        GLCall(glUniform1i(location, value);)
    }*/

    void Shader::SetUniform(const char* name, const glm::mat4& matrix) {
        s32 location = glGetUniformLocation(m_Handle, name);
        assert(location != -1);
        GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));)
    }

    void Shader::SetUniform(const char* name, s32* values, size_t count) {
        s32 location = glGetUniformLocation(m_Handle, name);
        assert(location != -1);
        GLCall(glUniform1iv(location, count, values);)
    }

    Shader::~Shader() {
        if (m_Handle) {
            GLCall(glDeleteProgram(m_Handle);)
        }
    }
}
