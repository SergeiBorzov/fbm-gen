#pragma once
#include <unordered_map>
#include <string>

#include <glm/glm.hpp>

#include "../Core/types.h"
#include "../Core/fbmgen_gl.h"

namespace fbmgen {        
    
    class Shader {
    public:
        static Shader* Load(const char* filename);

        inline u32 GetHandle() const { return m_Handle; }
        
        void Run();

        void SetUniform(const char* name, s32 value);
        void SetUniform(const char* name, s32* values, s32 count);
        void SetUniform(const char* name, f32 value);
        void SetUniform(const char* name, const glm::vec2& vector);
        void SetUniform(const char* name, const glm::vec3& vector);
        void SetUniform(const char* name, const glm::vec4& vector);
        void SetUniform(const char* name, const glm::mat3& matrix);
        void SetUniform(const char* name, const glm::mat4& matrix);

        ~Shader();
    private:
        bool Compile();
        bool CompilePart(const char* source, GLenum type);
        bool CompileSuccess(u32 handle);
        bool LinkSuccess();

        std::unordered_map<GLenum, std::string> m_ShaderMap;
        std::unordered_map<GLenum, u32> m_ShaderHandles;

        u32 m_Handle = 0;
    };
}