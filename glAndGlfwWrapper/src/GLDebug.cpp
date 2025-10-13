#include "GLDebug.hpp"
#include <iostream>

/**
 * @brief sets up debug messaging for OpenGL with the help of messageCallback as callback function
 */
void GLDebug::sEnable()
{
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback(GLDebug::sMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        std::cout << "[GLDebug] OpenGL debug output enabled" << std::endl;
    }
    else
    {
        std::cerr << "[GLDebug] Warning: Debug context not available. "
            << "Run with a debug OpenGL context to enable debug output."
            << std::endl;
    }
}

/**
 * @param source where the error might come from
 * @param type what kind of error were talking about
 * @param id the error id
 * @param severity the severity of the error
 * @param length unused
 * @param message the message from this error
 * @param userParam unused
 * @brief tries to format the error message based from it source, type, and severity, and prints that info its id and message
 */
void GLDebug::sMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
    std::string src, t, sev;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API: 
            src = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: 
            src = "Window";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            src = "ShaderCompiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            src = "3rdParty";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            src = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            src = "Other";
            break;
        default:
            src = "Unknown";
            break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            t = "error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            t = "Deprecated";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            t = "Undefined";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            t = "Portability";
            break;
        case GL_DEBUG_TYPE_MARKER:
            t = "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            t = "PushGroup";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            t = "PopGroup";
            break;
        case GL_DEBUG_TYPE_OTHER:
            t = "Other";
            break;
        default:
            t = "Unknown";
            break;
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            sev = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            sev = "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            sev = "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            sev = "NOTIFY";
            break;
        default:
            sev = "UNKNOWN";
            break;
    }

    std::ostream& out = (severity == GL_DEBUG_SEVERITY_HIGH) ? std::cerr : std::cout;
    out << "[OpenGL Debug] (" << id << ") "
        << "Source: " << src << ", "
        << "Type: " << t << ", "
        << "Severity: " << sev << "\n"
        << " Message: " << message << "\n";
}