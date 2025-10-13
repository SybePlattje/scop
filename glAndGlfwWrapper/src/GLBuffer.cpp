#include "GLBuffer.hpp"
#include <stdexcept>
#include <string>

GLBuffer::GLBuffer(e_Type type):
m_id(0),
m_type(type),
m_count(0)
{
    glGenBuffers(1, &m_id);
    if (0 == m_id)
        throw std::runtime_error(std::string("failed to make buffer of type: " + sToGLenum(type)));
}

GLBuffer::GLBuffer(GLBuffer&& other):
m_id(other.m_id),
m_type(other.m_type),
m_count(other.m_count)
{
    other.m_id = 0;
    other.m_count = 0;
}

GLBuffer::~GLBuffer()
{
    if (0 != m_id)
        glDeleteBuffers(1, &m_id);
}

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



void GLBuffer::bind() const
{
    glBindBuffer(sToGLenum(m_type), m_id);
}

void GLBuffer::unbind() const
{
    glBindBuffer(sToGLenum(m_type), 0);
}

GLuint GLBuffer::getId() const
{
    return m_id;
}

GLsizei GLBuffer::getCount() const
{
    return m_count;
}

GLBuffer::e_Type GLBuffer::getType() const
{
    return m_type;
}

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