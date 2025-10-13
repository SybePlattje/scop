#include "GLMesh.hpp"
#include <iostream>
#include <stdexcept>

GLMesh::GLMesh():
m_vertexArrayObject(0),
m_vertexCount(0),
m_indexCount(0)
{
    glGenVertexArrays(1, &m_vertexArrayObject);
    if (m_vertexArrayObject == 0)
        throw std::runtime_error("failed to create vertex array object");
}

GLMesh::GLMesh(GLMesh&& other):
m_vertexArrayObject(other.m_vertexArrayObject),
m_vertexCount(other.m_vertexCount),
m_indexCount(other.m_indexCount)
{
    other.m_vertexArrayObject = 0;
    other.m_vertexCount = 0;
    other.m_indexCount = 0;
}

GLMesh::~GLMesh()
{
    cleanup();
}

GLMesh& GLMesh::operator=(GLMesh&& other)
{
    if (this != &other)
    {
        cleanup();

        m_vertexArrayObject = other.m_vertexArrayObject;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;

        other.m_vertexArrayObject = 0;
        other.m_vertexCount = 0;
        other.m_indexCount = 0;
    }

    return *this;
}

/**
 * @attention attributes.offset and attributes.stride are assumed to hold there byte value
 * 
 */
bool GLMesh::attachVertexBuffer(const GLBuffer& buffer, std::vector<s_VertexAttribute>& attributes)
{
    if (buffer.getType() != GLBuffer::e_Type::Array)
    {
        std::cerr << "AttachVertexBuffer: buffer is not a vertex array buffer" << std::endl;
        return false;
    }

    if (attributes.empty())
    {
        std::cerr << "AttachVertexBuffer: attributes vector is empty" << std::endl;
        return false;
    }

    bind();
    buffer.bind();

    GLsizei strideBytes = 0;

    for (const s_VertexAttribute& att : attributes)
    {
        glEnableVertexAttribArray(att.index);
        glVertexAttribPointer(att.index, att.size, att.type, att.normalized, att.stride, reinterpret_cast<const void*>(att.offset));

        if (strideBytes < att.stride)
            strideBytes = att.stride;
    }

    std::size_t typeSize = getTypeSize(attributes[0].type);
    if (0 == typeSize)
    {
        std::cerr << "AttachVertexBuffer: unkown attachment type" << std::endl;
        unbind();
        return false;
    }

    if (typeSize > static_cast<std::size_t>(strideBytes))
    {
        std::cout << "resized strideByte(" << strideBytes << ") because it was smaller then typeSize(" << typeSize << ")" << std::endl;
        strideBytes = static_cast<GLsizei>(typeSize);
    }

    if (0 < strideBytes)
        m_vertexCount = static_cast<GLsizei>(buffer.getCount() / (strideBytes / typeSize));
    else
        m_vertexCount = 0;

    unbind();

    return true;
}

bool GLMesh::attachElementBuffer(const GLBuffer& buffer)
{
    if (buffer.getType() != GLBuffer::e_Type::Element)
    {
        std::cerr << "attachElementBuffer: buffer is not a element array buffer" << std::endl;
        return false;
    }

    bind();
    buffer.bind();
    m_indexCount = buffer.getCount();
    unbind();
    return true;
}

void GLMesh::bind() const
{
    glBindVertexArray(m_vertexArrayObject);
}

void GLMesh::unbind() const
{
    glBindVertexArray(0);
}

void GLMesh::draw(GLenum mode, GLsizei count, GLenum indexType) const
{
    bind();
    if (0 < m_indexCount)
    {
        GLsizei finalCount = (0 == count) ? m_indexCount : count;
        if (0 < finalCount)
            glDrawElements(mode, finalCount, indexType, nullptr);
    }
    else if (0 < m_vertexCount)
    {
        GLsizei finalCount = (0 == count) ? m_vertexCount : count;
        if (0 < finalCount)
            glDrawArrays(mode, 0, finalCount);
    }
    unbind();
}

GLuint GLMesh::getVertextArrayObject() const
{
    return m_vertexArrayObject;
}

void GLMesh::cleanup()
{
    if (m_vertexArrayObject)
    {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
        m_vertexArrayObject = 0;
    }
}

std::size_t GLMesh::getTypeSize(GLenum type)
{
    switch (type)
    {
    case GL_FLOAT:
        return sizeof(float);
    case GL_UNSIGNED_INT:
        return sizeof(unsigned int);
    case GL_INT:
        return sizeof(int);
    case GL_BYTE:
        return sizeof(char);
    case GL_UNSIGNED_BYTE:
        return sizeof(unsigned char);
    case GL_SHORT:
        return sizeof(short);
    case (GL_UNSIGNED_SHORT):
        return sizeof(unsigned short);
    case GL_DOUBLE:
        return sizeof(double);
    default:
        return 0;
    }
}