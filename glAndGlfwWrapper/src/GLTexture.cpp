#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"
#include "GLTexture.hpp"
#include "GLUtils.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

/**
 * @param a the fist vec_2 to check which is lower
 * @param b the second vec_2 to check which is lower
 * @return the lower of the vec2
 * @brief checks which of the 2 vec2 is the lower
 */
static s_vec2 sMinVec2(const s_vec2& a, const s_vec2& b)
{
    if (b.x > a.x && b.y > a.y)
        return a;
    return b;
}

/**
 * @param a the fist vec_2 to check which is higher
 * @param b the second vec_2 to check which is higher
 * @return the higher of the vec2
 * @brief checks which of the 2 vec2 is the higher
 */
static s_vec2 sMaxVec2(const s_vec2& a, const s_vec2& b)
{
    if (b.x < a.x && b.y < a.y)
        return a;
    return b;
}

/**
 * @param value the value to set between min and max
 * @param minval the minimum value value should have
 * @param maxval the maximum value value should have
 * @return value if value is between minVal and maxVal, maxVal if value is higher then, minVal is value is lower tehn minVal
 * @brief clamps the value between the min and max values 
 */
static s_vec2 sClamp(const s_vec2& value, float minVal = 0.f, float maxVal = 1.f)
{
    return 
    {
        std::max(minVal, std::min(value.x, maxVal)),
        std::max(minVal, std::min(value.y, maxVal))
    };
}

/**
 * @brief initializes all object variables
 */
GLTexture::GLTexture(): m_textureId(0), m_width(0), m_height(0), m_channels(0) {}

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
 * @param path the path to the texture
 * @brief initializes the texture
 * @return true if texture setup is successfull, false on failed
 */
bool GLTexture::setup(const std::string& path)
{
    if (!loadFromFile(path))
    {
        std::cerr << "failed to load texture" << std::endl;
        return false;
    }
    return true;
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
    {
        std::cerr << "failed to read texture file" << std::endl;
        return false;
    }

    buffer.emplace_back('\0');
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

/**
 * @param vertices the vector holding all the verrtices of the object
 * @param indices the vector holding all the faces of the object
 * @param out the s_vec2 vector that will hold the texure coordinates
 * @brief creates the texture coordinates so the texture is taking up 1 face
 */
void GLTexture::generateTexCoordPerFace(
    const std::vector<s_vec3>& vertices,
    const std::vector<unsigned int>& indices,
    std::vector<s_vec2>& out)
{
    out.clear();
    out.reserve(vertices.size());

    auto project = [&](const s_vec3& v, int majorAxis) -> s_vec2
    {
        switch (majorAxis)
        {
            case 0:
                return {v.y, v.z};
            case 1:
                return {v.x, v.z};
            default:
                return {v.x, v.y};
        }
    };

    for (std::size_t i = 0; i < indices.size(); i += 3)
    {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        s_vec3 v0 = vertices[i0];
        s_vec3 v1 = vertices[i1];
        s_vec3 v2 = vertices[i2];

        s_vec3 edge1 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
        s_vec3 edge2 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};

        s_vec3 faceNormal = {
            edge1.y * edge2.z - edge1.z * edge2.y,
            edge1.z * edge2.x - edge1.x * edge2.z,
            edge1.x * edge2.y - edge1.y * edge2.x
        };

        float len = std::sqrt(faceNormal.x * faceNormal.x + faceNormal.y * faceNormal.y + faceNormal.z * faceNormal.z);
        if (len < 1e-6f)
            continue;

        faceNormal.x /= len;
        faceNormal.y /= len;
        faceNormal.z /= len;

        s_vec3 absN{ std::abs(faceNormal.x), std::abs(faceNormal.y), std::abs(faceNormal.z)};

        int majorAxis = 0;
        if (absN.y > absN.x && absN.y > absN.z)
            majorAxis = 2;
        if (absN.z > absN.x && absN.z > absN.y)
            majorAxis = 1;

        s_vec2 uv0 = project(v0, majorAxis);
        s_vec2 uv1 = project(v1, majorAxis);
        s_vec2 uv2 = project(v2, majorAxis);

        s_vec2 minUV = sMinVec2(uv0, sMinVec2(uv1, uv2));
        s_vec2 maxUV = sMaxVec2(uv0, sMaxVec2(uv1, uv2));
        s_vec2 range{
            std::max(maxUV.x - minUV.x, 1e-6f),
            std::max(maxUV.y - minUV.y, 1e-6f)
        };

        out[i0] = {(uv0.x - minUV.x) / range.x, (uv0.y - minUV.y) / range.y};
        out[i1] = {(uv1.x - minUV.x) / range.x, (uv1.y - minUV.y) / range.y};
        out[i2] = {(uv2.x - minUV.x) / range.x, (uv2.y - minUV.y) / range.y};
    }
}

/**
 * @param vertices the vector holding all vertices of the object
 * @param indices the vector holding all faces of the object
 * @param out the s_vec2 vector that will hold the coordinates of the texture
 * @brief generate the texture coords to go over the hole object
 */
void GLTexture::generateTexCoordGlobal(
    const std::vector<s_vec3>& vertices,
    const std::vector<unsigned int>& indices,
    std::vector<s_vec2>& out)
{
    out.clear();
    out.reserve(vertices.size());

    auto project = [&](const s_vec3& v, int majorAxis) -> s_vec2
    {
        switch (majorAxis)
        {
            case 0:
                return {v.y, v.z};
            case 1:
                return {v.x, v.z};
            default:
                return {v.x, v.y};
        }
    };

    s_vec3 totalNormal{};
    for (std::size_t i = 0; i < indices.size(); i += 3)
    {
        const s_vec3& v0 = vertices[indices[i]];
        const s_vec3& v1 = vertices[indices[i + 1]];
        const s_vec3& v2 = vertices[indices[i + 2]];

        const s_vec3 min1 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
        const s_vec3 min2 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};

        const s_vec3 cross = {
            min1.y * min2.z - min1.z * min2.y,
            min1.z * min2.x - min1.x * min2.z,
            min1.x * min2.y - min1.y * min2.x
        };

        totalNormal = {
            totalNormal.x + cross.x,
            totalNormal.y + cross.y,
            totalNormal.z + cross.z
        };
    }

    s_vec3 absN{std::abs(totalNormal.x), std::abs(totalNormal.y), std::abs(totalNormal.z)};

    int majorAxis = 0;
    if (absN.y > absN.x && absN.y > absN.z)
        majorAxis = 2;
    if (absN.z > absN.x && absN.z > absN.y)
        majorAxis = 1;

    s_vec2 minBound{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    s_vec2 maxBound{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};

    for (const s_vec3& v: vertices)
    {
        s_vec2 p = project(v, majorAxis);
        minBound.x = std::min(minBound.x, p.x);
        minBound.y = std::min(minBound.y, p.y);
        maxBound.x = std::max(maxBound.x, p.x);
        maxBound.y = std::max(maxBound.y, p.y);
    }

    s_vec2 size = {
        std::max(maxBound.x - minBound.x, 1e-6f),
        std::max(maxBound.y - minBound.y, 1e-6f)
    };
    s_vec2 invSize = {1.f / size.x, 1.f / size.y};

    auto uv = [&](const s_vec3& v) {
        s_vec2 p = project(v, majorAxis);
        return sClamp({
            (p.x - minBound.x) * invSize.x,
            (p.y - minBound.y) * invSize.y
        });
    };

    for (std::size_t i = 0; i < indices.size(); i += 3)
    {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        out[i0] = uv(vertices[i0]);
        out[i1] = uv(vertices[i1]);
        out[i2] = uv(vertices[i2]);
    }
}