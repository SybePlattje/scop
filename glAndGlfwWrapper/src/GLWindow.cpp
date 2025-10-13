#include "GLWindow.hpp"
#include <stdexcept>
#include <iostream>
#include <algorithm>

/**
 * @param width the width the window will have in screen coordination, must be greater then 0
 * @param height the hegiht the window will have in screen coordination, must be greater then 0
 * @param title the title the screen window will have
 * @brief will create a window of the given width and height on the current monitor centered in the middle
 * @exception runtime_error if width or height is lower then 1
 * @exception runtime_error if glfwCreateWindow failes;
 */
GLWindow::GLWindow(int width, int height, const std::string& title):
m_title(title)
{
    if ( 0 > width || 0 > height)
        throw std::runtime_error("Width and height should both be greater then 0");

    m_width = width;
    m_height = height;
    
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        const char* err = nullptr;
        glfwGetError(&err);
        std::string msg = "Failed to create window";
        if (err)
            msg += std::string(": ") + err;
        throw std::runtime_error(msg);
    }
    
    makeWindowCurrent();
    swapIntervals(true);
    
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    if (!primary)
        throw std::runtime_error("Failed to find primary monitor");

    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    if (!mode)
        throw std::runtime_error("Failed to get monitor infor");

    int xpos = (mode->width - width);
    int ypos = (mode->height - height);
    glfwSetWindowPos(m_window, xpos, ypos);

    setWindowPointer(this);
    getFrameBuffer(&m_fbWidth, &m_fbHeight);
}

/**
 * @param title the title the window will have
 * @brief creates a fullscreen window on the primary monitor and sets it as current content
 * @exception runtime_error if monitor is not found or the library is not initialized
 * @exception runtime_error if the info of the monitor is not found or the library is not initialized
 * @exception runtime_error if glfwCreateWindow failes;
 */
GLWindow::GLWindow(const std::string& title):
m_title(title)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor)
        throw std::runtime_error("Failed to find primary monitor");
    
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode)
        throw std::runtime_error("Failed to get monitor info");

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    m_width = mode->width;
    m_height = mode->height;

    m_window = glfwCreateWindow(m_width, m_height, title.c_str(), monitor, nullptr);
    if (!m_window)
    {
        const char* err = nullptr;
        glfwGetError(&err);
        std::string msg = "Failed to create window";
        if (err)
            msg += std::string(": ") + err;
        throw std::runtime_error(msg);
    }

    makeWindowCurrent();
    swapIntervals(true);
    setWindowPointer(this);
    getFrameBuffer(&m_fbWidth, &m_fbHeight);
}

/**
 * @brief destroys the window if it was initialized
 * @warning the window must not be current on any other thread when going out of scope
 */
GLWindow::~GLWindow()
{
    if (m_window)
        glfwDestroyWindow(m_window);
}

/**
 * @brief checks if the should close flag for the window is true
 * @return true if window should close, else false
 */
bool GLWindow::shoulClose() const
{
    return glfwWindowShouldClose(m_window);
}

/**
 * @brief sets the flag that the window should close to true
 */
void GLWindow::setWindowClose()
{
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

/**
 * @brief swaps the buffer on the window to the one just drawn
 */
void GLWindow::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

/**
 * @param title the string holding the new title
 * @brief updates the window title to the new title
 */
void GLWindow::setTitle(const std::string& title)
{
    m_title = title;
    glfwSetWindowTitle(m_window, m_title.c_str());
}

/**
 * @param width the new screen width
 * @param height the new screen height
 * @brief updates the window size the the new one
 * @warning if any negative values are given, an error message is printed and the function returns early without updating
 */
void GLWindow::setSize(int width, int height)
{
    if (0 > width || 0 > height)
    {
        std::cerr << "width and height can't be less then 0" << std::endl;
        return;
    }

    m_width = width;
    m_height = height;

    glfwSetWindowSize(m_window, m_width, m_height);
    getFrameBuffer(&m_fbWidth, &m_fbHeight);
    glViewport(0, 0, m_width, m_height);
}

/**
 * @param width pointer that will hold the current window width
 * @param height pointer that will hold the current window height
 * @brief gets the windows current width and heights and puts it in the parameters
 */
void GLWindow::getSize(int& width, int& height) const
{
    glfwGetWindowSize(m_window, &width, &height);
}

/**
 * @param key the key that is checked if pressed
 * @return true if the key is currently pressed, false if not
 * @brief checks if the state of the key is GLFW_PRESS
 */
bool GLWindow::isKeyPressed(int key) const
{
    int state = glfwGetKey(m_window, key);
    if (state == GLFW_PRESS)
        return true;
    return false;
}

/**
 * @brief makes the current window the top visable window
 */
void GLWindow::makeWindowCurrent()
{
    glfwMakeContextCurrent(m_window);
}

/**
 * @param callback the new callback function that handles the keyboard input
 * @return the previous set callback function, null if not set before or the library was not initialized 
 */
GLFWkeyfun GLWindow::setKeyCallback(GLFWkeyfun callback)
{
    return glfwSetKeyCallback(m_window, callback);
}

/**
 * @param callback the new callback function that handles the cursor position
 * @return the previous set callback function, null if not set before or the library was not initialized 
 */
GLFWcursorposfun GLWindow::setCursorCallback(GLFWcursorposfun callback)
{
    return glfwSetCursorPosCallback(m_window, callback);
}

/**
 * @param red the anound of red in rgba value you want
 * @param green the amound of green in rgba value you want
 * @param blue the amound of blue in rgba value you want
 * @param alpha the amound of transparency you want
 * @brief sets the color of the entire window to the mix of rgba given
 */
void GLWindow::setClearColor(float red, float green, float blue, float alpha)
{
    glClearColor(
        std::clamp(red, 0.f, 1.f),
        std::clamp(green, 0.f, 1.f),
        std::clamp(blue, 0.f, 1.f),
        std::clamp(alpha, 0.f, 1.f)
    );
}

/**
 * @param color bool if we want to reset the color of the window
 * @param depth bool if we want to reset the depth of the window
 * @brief based on the parameters it will clear the window of set values
 */
void GLWindow::clear(bool color, bool depth) const
{
    if (color && depth)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    else if (color)
        glClear(GL_COLOR_BUFFER_BIT);
    else if (depth)
        glClear(GL_DEPTH_BUFFER_BIT);
}

/**
 * @param interval the amound of vertical blanking periods (V-syncs)
 * @brief set the interval buffer on how fast the new buffer will be refreshed where 0 is imidiatly, 1 is monitor refresh rate, 2 is half of monitor refresh rate, (refresh rate)/interval
 * @warning an error message will be printed if interval is less then 0 and returned early
 */
void GLWindow::swapIntervals(int interval)
{
    if (0 > interval)
    {
        std::cerr << "interval cannot be less then 0" << std::endl;
        return;
    }

    glfwSwapInterval(interval);   
}

/**
 * @param fbWidth pointer that will hold the framebuffer width
 * @param fbHeidht pointer that will hold the framebuffer height
 * @brief gets the framebuffer width and height form the current window
 */
void GLWindow::getFrameBuffer(int* fbWidth, int* fbHeight)
{
    glfwGetFramebufferSize(m_window, fbWidth, fbHeight);
}

