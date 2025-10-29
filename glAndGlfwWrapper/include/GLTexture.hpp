#ifndef GLTEXTURE_HPP
# define GLTEXTURE_HPP

#include <string>
#include <glad/glad.h>
#include <vector>
#include "GLShader.hpp"

class GLTexture
{
    public:
        GLTexture();
        GLTexture(GLTexture&& other);
        ~GLTexture();

        GLTexture& operator=(GLTexture&& other);

        bool setup(const std::string& path);
        bool loadFromFile(const std::string& path);
        void bind(unsigned int slot = 0) const;
        void unbind() const;
        void generateTexCoordPerFace(const std::vector<s_vec3>& vertices, const std::vector<unsigned int>& indices, std::vector<s_vec2>& out);
        void generateTexCoordGlobal(const std::vector<s_vec3>& vertices, const std::vector<unsigned int>& indices, std::vector<s_vec2>& out);
		GLuint getTextureId();
    private:
        GLuint m_textureId;
        int m_width;
        int m_height;
        int m_channels;

        void freeTexture();
};

#endif