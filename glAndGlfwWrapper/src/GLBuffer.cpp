#include "GLBuffer.hpp"
#include <stdexcept>
#include <string>

/**
 * @param type the type of buffer
 * @brief sets the type of buffer and the rest to default values
 */
GLBuffer::GLBuffer(e_Type type): m_id(0), m_type(type), m_count(0) {}

/**
 * @param other the buffer object with data to be moved
 * @brief moves the data to take ownership and sets the parameters to default
 */
GLBuffer::GLBuffer(GLBuffer&& other):
m_id(other.m_id),
m_type(other.m_type),
m_count(other.m_count)
{
    other.m_id = 0;
    other.m_count = 0;
}

/**
 * @brief buffer deconstructer, and deletes the gl buffer if set
 */
GLBuffer::~GLBuffer()
{
    if (0 != m_id)
        glDeleteBuffers(1, &m_id);
}

/**
 * @param other the buffer object with data to be moved
 * @brief takes the ownership of other data and sets other variables to 0
 * @return the moved GLBuffer
 */
GLBuffer& GLBuffer::operator=(GLBuffer&& other)
{
    if (this != &other)
    {
        if (0 != m_id)
            glDeleteBuffers(1, &m_id);
        
        m_id = other.m_id;
        m_type = other.m_type;
        m_count = other.m_count;

        other.m_id = 0;
        other.m_count = 0;
    }
    return *this;
}

/**
 * @brief generates the buffer
 * @return true if buffer generation is successfull, false if buffer generation fails
 */
bool GLBuffer::setup()
{
    glGenBuffers(1, &m_id);
    if (0 == m_id)
        return false;
    return true;
}

/**
 * @brief binds the buffer with glBindBuffer
 */
void GLBuffer::bind() const
{
    glBindBuffer(sToGLenum(m_type), m_id);
}

/**
 * @brief unbinds the buffer
 */
void GLBuffer::unbind() const
{
    glBindBuffer(sToGLenum(m_type), 0);
}

/**
 * @brief gets the id of buffer
 * @return the id of the buffer
 */
GLuint GLBuffer::getId() const
{
    return m_id;
}

/**
 * @brief gets the size of data that the buffer represents
 * @return the size of the data buffer it represents
 */
GLsizei GLBuffer::getCount() const
{
    return m_count;
}

/**
 * @brief gets the type of the buffer
 * @return the buffer type
 */
GLBuffer::e_Type GLBuffer::getType() const
{
    return m_type;
}

/**
 * @param type the type as e_Type
 * @brief converts the e_type to the GLenum of the same buffer type
 * @return the buffer type as GLenum
 */
GLenum GLBuffer::sToGLenum(e_Type type)
{
    switch (type)
    {
        case e_Type::Array:
            return GL_ARRAY_BUFFER;
        case e_Type::Element:
            return GL_ELEMENT_ARRAY_BUFFER;
        case e_Type::ShaderStorage:
            return GL_SHADER_STORAGE_BUFFER;
        case e_Type::Uniform:
            return GL_UNIFORM_BUFFER;
        default:
            return GL_ARRAY_BUFFER;
    }
}