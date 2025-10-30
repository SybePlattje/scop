#ifndef GLMESH_HPP
# define GLMESH_HPP

# include <vector>
# include <glad/glad.h>
# include "GLBuffer.hpp"

struct s_VertexAttribute
{
    GLuint index; // layout location
    GLint size; // e.g. 3. for vec3
    GLenum type; // GL_FLOAT, etc.
    GLboolean normalized;
    GLsizei stride; // in bytes
    std::size_t offset; // in bytes
};

class GLMesh
{
    public:
        GLMesh();
        GLMesh(const GLMesh& ohter) = delete;
        GLMesh(GLMesh&& other);
        ~GLMesh();

        GLMesh& operator=(const GLMesh& other) = delete;
        GLMesh& operator=(GLMesh&& other);

        bool setup();
        bool attachVertexBuffer(const GLBuffer& buffer, const std::vector<s_VertexAttribute>& attributes);
        bool attachElementBuffer(const GLBuffer& buffer);
        void draw(GLenum mode = GL_TRIANGLES, GLsizei count = 0, GLenum indexType = GL_UNSIGNED_INT) const;
        void bind() const;
        void unbind() const;

        GLuint getVertextArrayObject() const;
    private:
        GLuint m_vertexArrayObject;
        GLsizei m_vertexCount;
        GLsizei m_indexCount;

        void cleanup();
        std::size_t getTypeSize(GLenum type);
};

#endif