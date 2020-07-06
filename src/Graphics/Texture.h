#ifndef FLY_TEXTURE
#define FLY_TEXTURE

#include "../Core/types.h"

namespace fbmgen {
    class Texture {
    public:
        static Texture* Load(const char* filename);

        enum class WrapMode { Repeat, Mirrored, Edge, Border };
        enum class FilterMode { Nearest, Linear };

        inline u32 GetHandle() const { return m_Handle; }
        inline s32 GetWidth() const { return m_Width; }
        inline s32 GetHeight() const { return m_Height; }
        inline s32 GetChannels() const { return m_Channels; }
        inline WrapMode GetWrapMode() const { return m_WrapMode; }
        inline FilterMode GetFilterMode() const { return m_FilterMode; }

        void SetWrapMode(WrapMode mode);
        void SetFilterMode(FilterMode mode);

        void Bind() const;

        ~Texture();
    private:
        WrapMode m_WrapMode = WrapMode::Repeat;
        FilterMode m_FilterMode = FilterMode::Nearest;
        u32 m_Handle = 0; 
        s32 m_Width = 0;
        s32 m_Height = 0;
        s32 m_Channels = 0;
    };
}

#endif /* End of FLY_TEXTURE */