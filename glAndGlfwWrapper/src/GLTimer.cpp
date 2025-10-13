#include "GLTimer.hpp"
#include "GLContext.hpp"
#include <GLFW/glfw3.h>

/**
 * @brief initializes the timer, sets the last time to now and delta time to zero
 */
GLTimer::GLTimer():
m_lastTime(GLContext::sGetTime()),
m_deltaTime(0.0)
{}

/**
 * @brief deconstructor for the GLTimer class
 */
GLTimer::~GLTimer() {}

/**
 * @brief updates the delta time and last time
 */
void GLTimer::update()
{
    double currentTime = GLContext::sGetTime();

    m_deltaTime = currentTime - m_lastTime;
    m_lastTime = currentTime;
}

/**
 * @brief gives the delta time (time between 2 updates)
 * @return the time between the last 2 updates in seconds
 */
float GLTimer::getDeltaTime() const
{
    return static_cast<float>(m_deltaTime);
}

/**
 * @brief resets last time to now, and delta time to 0
 */
void GLTimer::reset()
{
    m_lastTime = GLContext::sGetTime();
    m_deltaTime = 0.0;
}