#include "GLShader.hpp"
#include <iostream>
#include <stdexcept>
#include "GLUtils.hpp"

/**
 * @brief sets object variables to default
 */
GLShader::GLShader(): m_program(0) {}

/**
 * @brief cleans the shader program when done using
 */
GLShader::~GLShader()
{
    if (m_program)
        glDeleteProgram(m_program);
}

/**
 * @param vertexFilePath the file path to the vertex shader
 * @param fragmentFilePath the file path to the fragment shader
 * @brief creats and links the saders to the shader program which is setup
 * @return true if the shader compailing and linking succeeds, false if it fails and a error message is printed
 */
bool GLShader::setup(const std::string& vertexFilePath, const std::string& fragmentFilePath)
{
    std::vector<unsigned char> fileSource;

    if (!GLUtils::sReadShaderFile(vertexFilePath.c_str(), fileSource))
    {
        std::cerr << "Failed to read vertex shader file" << std::endl;
        return false;
    }
    fileSource.emplace_back('\0');

    std::string vertexSource = reinterpret_cast<char*>(fileSource.data());
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == 0)
    {
        std::cerr << "Failed to compile vertexShader" << std::endl;
        return false;
    }
    
    fileSource.clear();

    if (!GLUtils::sReadShaderFile(fragmentFilePath.c_str(), fileSource))
    {
        glDeleteShader(vertexShader);
        std::cerr << "Failed to read fragment shader file" << std::endl;
        return false;
    }
    fileSource.emplace_back('\0');

    std::string fragmentSource = reinterpret_cast<char*>(fileSource.data());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        std::cerr << "Failed to compile fragmentShader" << std::endl;
        return false;
    }

    m_program = glCreateProgram();
    if (m_program == 0)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        std::cerr << "Failed to create shader program" << std::endl;
        return false;
    }

    attachShader(vertexShader);
    attachShader(fragmentShader);
    linkProgram();

    if (!checkCompileErrors(m_program, GL_LINK_STATUS, true))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_program);
        std::cerr << "Failed to link shaders to program" << std::endl;
        return false;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

/**
 * @brief binds the shader program of the object
 */
void GLShader::bind() const
{
    glUseProgram(m_program);
}

/**
 * @brief unbinds the current shader program
 */
void GLShader::unbind() const
{
    glUseProgram(0);
}

/**
 * @brief gives the id of the shader program
 * @return the id of the shader program
 */
GLuint GLShader::getProgramId() const
{
    return m_program;
}

/**
 * @param shader the shader being attached to the program
 * @brief attaches the shader to the program and puts it in a vector for cashing
 */
void GLShader::attachShader(GLuint shader)
{
    glAttachShader(m_program, shader);
    m_shaders.emplace_back(shader);
}

/**
 * @brief links the shader program and the shaders
 * @return true if linking is successfull, else false is returned 
 */
bool GLShader::linkProgram()
{
    glLinkProgram(m_program);
    if (!checkCompileErrors(m_program, GL_LINK_STATUS, true))
    {
        std::cerr << "failed to link shader to program" << std::endl;
        return false;
    }

    for (GLuint shader : m_shaders)
        glDetachShader(m_program, shader);
    m_shaders.clear();
    return true;
}

/**
 * @param name the name of the uniform
 * @brief tries to find the uniform of the given name in the shaderprogram, if found its stored in a map for easy lookup later
 * @return an integer represending the id of where the uniform was found, or -1 if not found or error
 */
GLint GLShader::getUniformLocation(const std::string& name)
{
    if (m_uniformCache.find(name) != m_uniformCache.end())
        return m_uniformCache[name];
    
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location == -1)
    {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist" << std::endl;
        return -1;
    }

    m_uniformCache.emplace(name, location);
    return location;
}

/**
 * @param type the type of shader as enum given by OpenGL
 * @brief changes the enum type to the sting type of the shader type
 * @return the string version of the shadertype, UNKNOWN is return if no type matches
 */
std::string GLShader::sShaderTypeToString(GLenum type)
{
    switch (type)
    {
        case GL_VERTEX_SHADER:
            return "VERTEX";
        case GL_FRAGMENT_SHADER:
            return "FRAGMENT";
        case GL_GEOMETRY_SHADER:
            return "GEOMETRY";
        case GL_COMPUTE_SHADER:
            return "COMPUTE";
        case GL_TESS_CONTROL_SHADER:
            return "TESS_CONTROL";
        case GL_TESS_EVALUATION_SHADER:
            return "TESS_EVALUATION";
        default:
            return "UNKNOWN";
    }
}

/**
 * @param type the type of shader
 * @param source the source data of the shader
 * @brief tries to compile the shader into the given shader type using the source
 * @return the id of the compiled shader, or 0 on failure
 */
GLuint GLShader::compileShader(GLenum type, const std::string& source)
{
    const char* src = source.c_str();
    GLuint shader = glCreateShader(type);
    
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    if (!checkCompileErrors(shader, type, false))
        return 0;
    
    return shader;
}

/**
 * @param object the object that is being checked on errors
 * @param type what type of error were checking on
 * @param isProgram flag for use of program or shader
 * @brief checks if the given object has errors regarding the type
 * @return true if no errors are found, else false and a message is printed
 */
bool GLShader::checkCompileErrors(GLuint object, GLenum type, bool isProgram)
{
    GLint success = 0;
    char buffer[512];

    if (!isProgram)
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 512, nullptr, buffer);
            std::cerr << "❌ SHADER_COMPILATION_ERROR of type: " << sShaderTypeToString(type) << "\n" << buffer << std::endl;
            return false;
        }
        return true;
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 512, nullptr, buffer);
            std::cerr << "❌ PROGRAM_LINKING_ERROR\n" << buffer << std::endl;
            return false;
        }
        return true;
    }
    return false;
}
