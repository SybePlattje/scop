#ifndef GLCONTEXT_HPP
# define GLCONTEXT_HPP

# include <GLFW/glfw3.h>

class GLContext
{
    public:
        GLContext(int major = 4, int minor = 1);
        ~GLContext();

        static bool sInitGlad();
        static void sPollEvents();
        static void sWaitEvents();
        static double sGetTime();
        static GLFWerrorfun sSetErrorCallback(GLFWerrorfun callback);
        static void sTerminate();
    private:
        bool initGlfw();
        void sanitizeVersions(int& major, int& minor);
};

#endif