#ifndef GLWINDOW_HPP
# define GLWINDOW_HPP

# include <GLFW/glfw3.h>
# include <string>

class GLWindow
{
    public:
        GLWindow(int width, int height, const std::string& title);
        GLWindow(const std::string& title);
        GLWindow(const GLWindow& other) = delete;
        ~GLWindow();

        const GLWindow& operator=(const GLWindow& other) = delete;

        bool shoulClose() const;
        void setWindowClose();
        void swapBuffers() const;
        void setTitle(const std::string& title);
        void setSize(int width, int height);
        void getSize(int& width, int& height) const;

        bool isKeyPressed(int key) const;
        GLFWkeyfun setKeyCallback(GLFWkeyfun callback);
        GLFWcursorposfun setCursorCallback(GLFWcursorposfun callback);

        void setClearColor(float red, float green, float blue, float alpha);
        void clear(bool color = true, bool depth = true) const;

        void makeWindowCurrent();
        void swapIntervals(int interval);
        void getFrameBuffer(int* fbWidth, int* fbHeight);
        
        /**
         * @param data the data pointer you want to add to add to the window for functions to use later
         * @brief makes the data later available in the window for other functons to get
         */
        template<typename T>
        void setWindowPointer(T* data)
        {
            glfwSetWindowUserPointer(m_window, static_cast<void*>(data));
        }

        /**
         * @return the datatype of the stored user pointer stored in window or NULL if non where set
         * @brief gets the window pointer the user added to the window
         */
        template<typename T>
        T* getWindowPointer()
        {
            return static_cast<T*>(glfwGetWindowUserPointer(m_window));
        }
    private:
        GLFWwindow* m_window;
        int m_width;
        int m_height;
        int m_fbWidth;
        int m_fbHeight;
        std::string m_title;


};

#endif