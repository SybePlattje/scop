#include <glad/glad.h>
#include "GLContext.hpp"
#include <stdexcept>
#include <iostream>

/**
 * @param major the major version of glfw you want to use
 * @param minor the minor version of glfw you want to use
 * @brief initializes glfw and sets the glfw version to the one given after senitizing to make sure it works and is a valid version
 * @exception runtime error if glfwInit failes
 */
GLContext::GLContext(int major, int minor)
{
    if (!initGlfw())
        throw std::runtime_error("Failed to initialze glfw");

    sanitizeVersions(major, minor);

    glfwWindowHint(GLFW_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_VERSION_MINOR, minor);
}

/**
 * @brief terminats glfw as cleanup
 */
GLContext::~GLContext()
{
    sTerminate();
}

/**
 * @brief terminates glfw
 */
void GLContext::sTerminate()
{
    glfwTerminate();
}

/**
 * @brief tries to load in all the glad function
 * @return true if glad initializes correctly, false on gladLoadGLLoader error
 */
bool GLContext::sInitGlad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "[GLAD] Failed to initialize OpenGL context!" << std::endl;
        return false;
    }
    #ifdef DEBUG
        std::cout << "[GLAD] OpenGL loaded: " 
            << glGetString(GL_VENDOR) << " | "
            << glGetString(GL_RENDERER) << " | "
            << glGetString(GL_VERSION) << std::endl;
    #endif
    return true;
}

/**
 * @brief tries to initialize glfw
 * @return ture if glfwInit succeeds, fails on glfwInit error and prints error message
 */
bool GLContext::initGlfw()
{
    if (glfwInit())
        return true;

    const char* errorDescription = nullptr;
    glfwGetError(&errorDescription);
    if (errorDescription)
        std::cerr << "[GLFW] Initialization failed: " << errorDescription << "\n";
    else
        std::cerr << "[GLFW] Initialization failed: Unknown error" << "\n";

    return false;
}

/**
 * @brief polls the events from the current window loop
 */
void GLContext::sPollEvents()
{
    glfwPollEvents();
}

/**
 * @brief gets the current time in seconds
 * @return the time in seconds or 0 if an error has occurred
 */
double GLContext::sGetTime()
{
    return glfwGetTime();
}

/**
 * @param callback the callback function for handling errors
 * @brief sets the given callback function to handle the errors
 * @return the previous callback function or null if non was set
 */
GLFWerrorfun GLContext::sSetErrorCallback(GLFWerrorfun callback)
{
    return glfwSetErrorCallback(callback);
}

/**
 * @param major the major version of glfw to senitize
 * @param minor the minor version of glfw to senitize
 * @brief senitizes the version of glfw to the minimum of 3.3, or maximan of 4.6, or the heighest version that's installed on the machine
 */
void GLContext::sanitizeVersions(int& major, int& minor)
{
    int glfwMajor, glfwMinor, glfwRev;
    glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRev);

    if (3 > major || (3 == major && 3 > minor))
    {
        major = 3;
        minor = 3;
    }

    if (4 == major && 6 < minor)
        minor = 6;
    if (4 == major && 3 < minor)
        minor = 3;
    
    if (glfwMajor < major)
    {
        major = glfwMajor;
        minor = glfwMinor;
    }
    else if ( glfwMajor == major && glfwMinor < minor)
        minor = glfwMinor;
}