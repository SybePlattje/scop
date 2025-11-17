#include "Scop.hpp"
#include "Utils.hpp"
#include "stdexcept"
#include <algorithm>

Scop::Scop(char* objectFilePath):
m_context(4, 1),
m_window(800, 800, "scop"),
m_shader(),
m_texture(),
m_buffers()
{
    m_info = Utils::sParseInput(objectFilePath);
    m_bbox = Utils::sComputeBoundingBoxAndScale(m_info.vertices);
    float boundingRadius = Utils::sBoundingBoxRadius(m_bbox);
    m_bbox.scale = 1.f / (2.f * boundingRadius);

    if (!GLContext::sInitGlad())
        throw std::runtime_error("failed to initialize glad");

    if (!m_shader.setup("shaders/vertex/source.vert", "shaders/fragment/source.frag"))
        throw std::runtime_error("failed to setup shaders");

    if (!m_texture.setup("textures/nyan.bmp"))
        throw std::runtime_error("failed to setup texture");

    std::vector<s_Vertex>verticesInterLeaved = setupShaderBufferData();

    std::vector<s_VertexAttribute> attributes;
    attributes.emplace_back(0, 3, GL_FLOAT, GL_FALSE, sizeof(s_Vertex), offsetof(s_Vertex, position));
    attributes.emplace_back(1, 2, GL_FLOAT, GL_FALSE, sizeof(s_Vertex), offsetof(s_Vertex, texCoord));
    attributes.emplace_back(2, 3, GL_FLOAT, GL_FALSE, sizeof(s_Vertex), offsetof(s_Vertex, normal));

    if (!setupBuffersGlobal(verticesInterLeaved, attributes))
        throw std::runtime_error("failed to setup buffers with global shaders");

    std::vector<s_Vertex> verticesInterLeavedFace = setupShaderBufferDataPerFace();

    if (!setupBuffersPerFace(verticesInterLeavedFace, attributes))
        throw std::runtime_error("failed to setup buffers with per face shader");

    m_displayInfo.transform.orientation = Utils::sQuatIdentify();

    m_window.setKeyCallback(smKeyCallback);
    m_window.setWindowPointer(&m_displayInfo);
    m_window.enable(false, true);
}

void Scop::start()
{
    float fovRadians = Utils::sRadiance();
    float boundingRadius = Utils::sBoundingBoxRadius(m_bbox);
    float distance = Utils::sDistance(boundingRadius, fovRadians) * 0.5f;

    s_vec3 up = {0.f, 1.f, 0.f};
    float near = 0.01f;
    float far = 100.f;

    m_window.setClearColor(0.4f, 0.2f, 0.8f, 1.f);
    while (!m_window.shoulClose())
    {
        m_timer.update();
        m_window.clear();
        m_window.enable(false, true);
        m_shader.bind();

        m_displayInfo.transform.mvp = setupModelViewProjection(fovRadians, near, far, distance, up);

        if (m_displayInfo.render.useTexture)
            m_displayInfo.render.blendValue += 1.f * m_timer.getDeltaTime();
        else
            m_displayInfo.render.blendValue -= 1.f * m_timer.getDeltaTime();
        
        m_displayInfo.render.blendValue = std::clamp(m_displayInfo.render.blendValue, 0.f, 1.f);

        m_texture.bind();
        m_shader.setUniform("uTexture", 0);
        m_shader.setUniform("uMVP", m_displayInfo.transform.mvp);
        m_shader.setUniform("uModel", m_displayInfo.transform.model);
        m_shader.setUniform("uBlend", m_displayInfo.render.blendValue);

        if (m_displayInfo.render.perFace)
            m_buffers.vaoFace.draw(GL_TRIANGLES, m_info.facesPerFace.size(), GL_UNSIGNED_INT);
        else
            m_buffers.vao.draw(GL_TRIANGLES, m_info.faces.size(), GL_UNSIGNED_INT);

        GLenum err = GL_NO_ERROR;
        while ((err = glGetError()) != GL_NO_ERROR)
            std::cerr << "GL error: " << err << std::endl;
        
        m_window.swapBuffers();
        GLContext::sPollEvents();
    }
}

std::vector<s_Vertex> Scop::setupShaderBufferData()
{
    std::vector<s_vec2> textureCoords;
    m_texture.generateTexCoordGlobal(m_info.vertices, m_info.faces, textureCoords);

    std::vector<s_vec3> normals = Utils::sComputeVertexNormals(m_info.vertices, m_info.faces);
    
    std::vector <s_Vertex> verticesInterLeaved;
    verticesInterLeaved.reserve(m_info.vertices.size());
    for (std::size_t i = 0; i < m_info.vertices.size(); ++i)
    {
        verticesInterLeaved.emplace_back(
            m_info.vertices[i],
            textureCoords[i],
            normals[i]
        );
    }

    return verticesInterLeaved;
}

std::vector<s_Vertex> Scop::setupShaderBufferDataPerFace()
{
    std::vector<s_vec2> textCoordFace;
    m_texture.generateTexCoordPerFace(m_info.facesPerFace, textCoordFace);

    std::vector<s_vec3> normalsFace = Utils::sComputeVertexNormals(m_info.verticesPerFace, m_info.facesPerFace);
    
    std::vector<s_Vertex> verticesInterLeavedFace;
    verticesInterLeavedFace.reserve(m_info.verticesPerFace.size());
    for (std::size_t i = 0; i < m_info.verticesPerFace.size(); ++i)
    {
        verticesInterLeavedFace.emplace_back(
            m_info.verticesPerFace[i],
            textCoordFace[i],
            normalsFace[i]
        );
    }

    return verticesInterLeavedFace;
}

bool Scop::setupBuffersGlobal(const std::vector<s_Vertex>& vertices, const std::vector<s_VertexAttribute>& attributes)
{
    if (!m_buffers.vbo.setup())
    {
        std::cerr << "failed to setup vbo buffer" << std::endl;
        return false;
    }
    if (!m_buffers.vao.setup())
    {
        std::cerr << "failed to setup vao buffer" << std::endl;
        return false;
    }
    if (!m_buffers.ebo.setup())
    {
        std::cerr << "failed to setup ebo buffer" << std::endl;
        return false;
    }

    if (!m_buffers.vbo.setData(vertices, GL_STATIC_DRAW))
    {
        std::cerr << "failed to set vbo data" << std::endl;
        return false;
    }

    if (!m_buffers.vao.attachVertexBuffer(m_buffers.vbo, attributes))
    {
        std::cerr << "failed to attach vbo to vao" << std::endl;
        return false;
    }

    if (!m_buffers.ebo.setData(m_info.faces, GL_STATIC_DRAW))
    {
        std::cerr << "failed to set ebo data" << std::endl;
        return false;
    }

    if (!m_buffers.vao.attachElementBuffer(m_buffers.ebo))
    {
        std::cerr << "failed to attach ebo to vbo" << std::endl;
        return false;
    }
    return true;
}

bool Scop::setupBuffersPerFace(const std::vector<s_Vertex>& vertices, const std::vector<s_VertexAttribute>& attributes)
{
    if (!m_buffers.vboFace.setup())
    {
        throw std::runtime_error("failed to setup vboFace buffer");
    }
    if (!m_buffers.vaoFace.setup())
    {
        throw std::runtime_error("failed to setup vaoFace buffer");
    }
    if (!m_buffers.eboFace.setup())
    {
        throw std::runtime_error("failed to setup eboFace buffer");
    }

    if (!m_buffers.vboFace.setData(vertices, GL_STATIC_DRAW))
    {
        throw std::runtime_error("failed to set vboFace data");
    }
    
    if (!m_buffers.vaoFace.attachVertexBuffer(m_buffers.vboFace, attributes))
    {
        throw std::runtime_error("failed to attach vboFace to vaoFace");
    }

    if (!m_buffers.eboFace.setData(m_info.facesPerFace, GL_STATIC_DRAW))
    {
        throw std::runtime_error("failed to set eboFace data");
    }

    if (!m_buffers.vaoFace.attachElementBuffer(m_buffers.eboFace))
    {
        throw std::runtime_error("failed to attack eboFace to vboFace");
    }

    return true;
}

s_mat4 Scop::setupModelViewProjection(float fovRadian, float near, float far, float distance, const s_vec3& up)
{
    int height;
    int width;

    m_window.getFrameBuffer(&width, &height);
    if (height == 0)
        height = 1;
    float aspect = static_cast<float>(width) / static_cast<float>(height);

    s_vec3 eye = {m_bbox.center.x, m_bbox.center.y, m_bbox.center.z - (distance * m_displayInfo.transform.zoomFactor)};

    s_mat4 view = Utils::sMat4LookAt(eye, m_bbox.center, up);
    s_mat4 proj = Utils::sMat4Perspective(fovRadian, aspect, near, far);

    s_mat4 T1 = Utils::sMat4Translate(-m_bbox.center.x, -m_bbox.center.y, -m_bbox.center.z);
    s_mat4 T2 = Utils::sMat4Translate(m_bbox.center.x, m_bbox.center.y, m_bbox.center.z);
    s_mat4 scale = Utils::sMat4Scale(m_bbox.scale);

    s_mat4 rotation = Utils::sQuatToMat4(m_displayInfo.transform.orientation);

    m_displayInfo.transform.model = Utils::sMat4Multiply(T2, Utils::sMat4Multiply(rotation, Utils::sMat4Multiply(scale, T1)));
    s_mat4 MVP = Utils::sMat4Multiply(proj, Utils::sMat4Multiply(view, m_displayInfo.transform.model));

    return MVP;
}

void Scop::smKeyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    s_DisplayInfo* dInfo = static_cast<s_DisplayInfo*>(glfwGetWindowUserPointer(window));
    if (!dInfo)
        return;

    s_vec3 axisX = {1.f, 0.f, 0.f};
    s_vec3 axisY = {0.f, 1.f, 0.f};
    s_vec3 axisZ = {0.f, 0.f, 1.f};

    const float angleStep = 0.01f;
    s_quat q = {};
    switch (key)
    {
        case GLFW_KEY_W: // up
            q = Utils::sQuatFromAxisAngle(axisX, angleStep);
            dInfo->transform.orientation = Utils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
         case GLFW_KEY_S: //down
            q = Utils::sQuatFromAxisAngle(axisX, -angleStep);
            dInfo->transform.orientation = Utils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
        case GLFW_KEY_D: // right
            q = Utils::sQuatFromAxisAngle(axisY, angleStep);
            dInfo->transform.orientation = Utils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
        case GLFW_KEY_A: // left
            q = Utils::sQuatFromAxisAngle(axisY, -angleStep);
            dInfo->transform.orientation = Utils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
        case GLFW_KEY_Q: // rotate left
            q = Utils::sQuatFromAxisAngle(axisZ, -angleStep);
            dInfo->transform.orientation = Utils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
        case GLFW_KEY_E: //rotate right
            q = Utils::sQuatFromAxisAngle(axisZ, angleStep);
            dInfo->transform.orientation = Utils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
        case GLFW_KEY_R: // reset
            dInfo->transform.orientation = Utils::sQuatIdentify();
            break;
        case GLFW_KEY_ESCAPE: // exit
            glfwSetWindowShouldClose(window, 1);
            break;
        case GLFW_KEY_EQUAL: // zoom in
            dInfo->transform.zoomFactor = std::max(dInfo->transform.minZoom, dInfo->transform.zoomFactor - dInfo->transform.zoomStep);
            break;
        case GLFW_KEY_MINUS: // zoom out
            dInfo->transform.zoomFactor = std::min(dInfo->transform.maxZoom, dInfo->transform.zoomFactor + dInfo->transform.zoomStep);
            break;
        case GLFW_KEY_T: // change color/texture
            dInfo->render.useTexture = !dInfo->render.useTexture;
            break;
        case GLFW_KEY_F:
            dInfo->render.perFace = !dInfo->render.perFace;
            break;
    }
    dInfo->transform.orientation = Utils::sQuatNormalize(dInfo->transform.orientation);
}