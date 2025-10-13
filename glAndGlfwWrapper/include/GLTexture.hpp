#ifndef GLTEXTURE_HPP
# define GLTEXTURE_HPP

#include <string>
#include <glad/glad.h>

class GLTexture
{
    public:
        GLTexture(const std::string& path);
        GLTexture(GLTexture&& other);
        ~GLTexture();

        GLTexture& operator=(GLTexture&& other);

        bool loadFromFile(const std::string& path);
        void bind(unsigned int slot = 0) const;
        void unbind() const;
        GLuint getTextureId();
    private:
        GLuint m_textureId;
        int m_width;
        int m_height;
        int m_channels;

        void freeTexture();
};

#endif