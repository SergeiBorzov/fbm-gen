#ifndef FLY_SHADER
#define FLY_SHADER

#include <unordered_map>

#include <glm/glm.hpp>

#include "../Core/types.h"
#include "../Core/fbmgen_gl.h"


namespace fbmgen {        
    
    class Shader {
    public:
        static Shader* Load(const char* filename);

        inline u32 GetHandle() const { return m_Handle; }
        
        void Run();

        void SetUniform(const char* name, const glm::mat4& matrix);
        void SetUniform(const char* name, s32* values, size_t count);


        ~Shader();
    private:
        bool Compile();
        bool CompilePart(const char* filename, GLenum type);
        bool CompileSuccess(u32 handle);
        bool LinkSuccess();


        std::unordered_map<GLenum, const char*> m_ShaderMap;
        std::unordered_map<GLenum, u32> m_ShaderHandles;

        u32 m_Handle = 0;
    };
}

#endif /* End of FLY_SHADER */