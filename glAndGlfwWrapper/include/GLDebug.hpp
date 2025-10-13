#ifndef GLDEBUG_HPP
# define GLDEBUG_HPP

# include <glad/glad.h>

class GLDebug
{
    public:
        static void sEnable();
        static void sMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};

#endif