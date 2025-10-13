#ifndef GLBUFFER_HPP
# define GLBUFFER_HPP

# include <glad/glad.h>
# include <vector>
# include <iostream>

class GLBuffer
{
    public:
        enum class e_Type
        {
            Array,
            Element,
            ShaderStorage,
            Uniform
        };

        GLBuffer(e_Type type = e_Type::Array);
        GLBuffer(const GLBuffer& other) = delete;
        GLBuffer(GLBuffer&& other);
        ~GLBuffer();
        
        GLBuffer& operator=(const GLBuffer& other) = delete;
        GLBuffer& operator=(GLBuffer&& other);

        void bind() const;
        void unbind() const;
        
        template<typename T>
        bool setData(const std::vector<T>&data, GLenum usage = GL_STATIC_DRAW)
        {
            if (data.empty())
            {
                std::cerr << "setData: data vector cannot be empty" << std::endl;
                return false;
            }

            bind();
            glBufferData(sToGLenum(m_type), data.size() * sizeof(T), data.data(), usage);
            m_count = static_cast<GLsizei>(data.size());
            return true;
        }

        GLuint getId() const;
        GLsizei getCount() const;
        e_Type getType() const;
    private:
        GLuint m_id;
        e_Type m_type;
        GLsizei m_count;

        static GLenum sToGLenum(e_Type type);
};

#endif