#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"
#include "GLTexture.hpp"
#include "GLUtils.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>

/**
 * @param path the path to the texture
 * @brief initializes the texture object
 * @exception runtime error if loading of texture fails
 */
GLTexture::GLTexture(const std::string& path):
m_textureId(0),
m_width(0),
m_height(0),
m_channels(0)
{
    if (!loadFromFile(path))
        throw std::runtime_error("failed to load texture");
}

/**
 * @param other the old texture object
 * @brief moves the data from the old object to the new one and empties data of the old object
 */
GLTexture::GLTexture(GLTexture&& other):
m_textureId(other.m_textureId),
m_width(other.m_width),
m_height(other.m_height),
m_channels(other.m_channels)
{
    other.m_textureId = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_channels = 0;
}

/**
 * @param other the old texture object
 * @brief moves the data from the old object to the new one and empties data of the old object
 * @return the new object
 */
GLTexture& GLTexture::operator=(GLTexture&& other)
{
    if(this != &other)
    {
        freeTexture();

        m_textureId = other.m_textureId;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;

        other.m_textureId = 0;
        other.m_width = 0;
        other.m_height = 0;
        other.m_channels = 0;
    }
    return *this;
}

/**
 * @brief frees the texture id for cleanup
 */
GLTexture::~GLTexture()
{
    freeTexture();
}

/**
 * @brief gives the id of the texture
 * @return the texture id
 */
GLuint GLTexture::getTextureId()
{
    return m_textureId;
}

/**
 * @param path the path to the texture
 * @brief loads the texture info from the path and makes it ready for use
 * @return true if texture was loaded in correctly, false if reading into the file or decoding it fails
 */
bool GLTexture::loadFromFile(const std::string& path)
{
    freeTexture();

    std::vector<unsigned char> buffer;
    if(!GLUtils::sReadTexture(path.c_str(), buffer))
        return false;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load_from_memory(buffer.data(), static_cast<int>(buffer.size()), &m_width, &m_height, &m_channels, 0);

    if (!data)
    {
        std::cerr << "failed to decode image: " << path << std::endl;
        return false;
    }

    GLenum format = GL_RGB;
    if (1 == m_channels)
        format = GL_RED;
    else if (3 == m_channels)
        format = GL_RGB;
    else if (4 == m_channels)
        format = GL_RGBA;

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    data = nullptr;

    return true;
}

/**
 * @param slot the slot the texture will hold
 * @brief activates the texture on the given slot and bind the texture
 */

void GLTexture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

/**
 * @brief unbinds the texture
 */
void GLTexture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief frees the texture id data
 */
void GLTexture::freeTexture()
{
    if (0 != m_textureId)
    {
        glDeleteTextures(1, &m_textureId);
        m_textureId = 0;
    }
}
