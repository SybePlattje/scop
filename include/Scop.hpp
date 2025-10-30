#ifndef SCOP_HPP
# define SCOP_HPP

# include "Struct.hpp"
# include "GLContext.hpp"
# include "GLWindow.hpp"
# include "GLShader.hpp"
# include "GLTexture.hpp"
# include "GLTimer.hpp"

class Scop
{
    public:
        Scop(char* objectFilePath);
        ~Scop() = default;
        void start();
    private:
        GLContext m_context;
        GLWindow m_window;
        GLShader m_shader;
        GLTexture m_texture;
        GLTimer m_timer;
        s_Buffers m_buffers;
        s_InputFileLines m_info;
        s_BoundingBox m_bbox;
        s_DisplayInfo m_displayInfo;

        std::vector<s_Vertex> setupShaderBufferData();
        std::vector<s_Vertex> setupShaderBufferDataPerFace();
        bool setupBuffersGlobal(const std::vector<s_Vertex>& vertices, const std::vector<s_VertexAttribute>& attributes);
        bool setupBuffersPerFace(const std::vector<s_Vertex>& veritces, const std::vector<s_VertexAttribute>& attributes);
        s_mat4 setupModelViewProjection(float fovRadian, float near, float far, float distance, const s_vec3& up);
        static void smKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

};

#endif