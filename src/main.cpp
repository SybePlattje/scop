#include "FileReader.hpp"
#include <glad/glad.h>
#include "GLContext.hpp"
#include "GLWindow.hpp"
#include "GLBuffer.hpp"
#include "GLMesh.hpp"
#include "GLShader.hpp"
#include "GLTexture.hpp"
#include "GLTimer.hpp"
#include "GLUtils.hpp"
#include "MathUtils.hpp"
#include "Struct.hpp"
#include <iostream>
#include <algorithm>

void smKeyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
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
            q = MathUtils::sQuatFromAxisAngle(axisX, angleStep);
            dInfo->transform.orientation = MathUtils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
         case GLFW_KEY_S: //down
            q = MathUtils::sQuatFromAxisAngle(axisX, -angleStep);
            dInfo->transform.orientation = MathUtils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
        case GLFW_KEY_D: // right
            q = MathUtils::sQuatFromAxisAngle(axisY, angleStep);
            dInfo->transform.orientation = MathUtils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
        case GLFW_KEY_A: // left
            q = MathUtils::sQuatFromAxisAngle(axisY, -angleStep);
            dInfo->transform.orientation = MathUtils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
        case GLFW_KEY_Q: // rotate left
            q = MathUtils::sQuatFromAxisAngle(axisZ, -angleStep);
            dInfo->transform.orientation = MathUtils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
        case GLFW_KEY_E: //rotate right
            q = MathUtils::sQuatFromAxisAngle(axisZ, angleStep);
            dInfo->transform.orientation = MathUtils::sQuatMultiply(dInfo->transform.orientation, q);
            break;
        case GLFW_KEY_R: // reset
            dInfo->transform.orientation = MathUtils::sQuatIdentify();
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
    dInfo->transform.orientation = MathUtils::sQuatNormalize(dInfo->transform.orientation);
}

s_mat4 setupModelViewProjection(float fovRadian, float near, float far, float distance, const s_vec3& up, GLWindow& window, const s_BoundingBox& bbox, s_DisplayInfo& dInfo)
{
    int height;
    int width;

    window.getFrameBuffer(&width, &height);
    if (height == 0)
        height = 1;
    float aspect = static_cast<float>(width) / static_cast<float>(height);

    s_vec3 eye = {bbox.center.x, bbox.center.y, bbox.center.z - (distance * dInfo.transform.zoomFactor)};

    s_mat4 view = MathUtils::sMat4LookAt(eye, bbox.center, up);
    s_mat4 proj = MathUtils::sMat4Perspective(fovRadian, aspect, near, far);

    s_mat4 T1 = MathUtils::sMat4Translate(-bbox.center.x, -bbox.center.y, -bbox.center.z);
    s_mat4 T2 = MathUtils::sMat4Translate(bbox.center.x, bbox.center.y, bbox.center.z);
    s_mat4 scale = MathUtils::sMat4Scale(bbox.scale);

    s_mat4 rotation = MathUtils::sQuatToMat4(dInfo.transform.orientation);

    dInfo.transform.model = MathUtils::sMat4Multiply(T2, MathUtils::sMat4Multiply(rotation, MathUtils::sMat4Multiply(scale, T1)));
    s_mat4 MVP = MathUtils::sMat4Multiply(proj, MathUtils::sMat4Multiply(view, dInfo.transform.model));

    return MVP;
}

int main(int argc, char* argv[])
{
    if (2 != argc)
    {
        std::cout << "to start use program like this\n ./scop NAME.obj" << std::endl;
        return 1;
    }

    try
    {
        s_InputFileLines info = FileReader::sParseInput(argv[1]);

        s_BoundingBox bbox = MathUtils::sComputeBoundingBoxAndScale(info.vertices);
        float boundingRadius = MathUtils::sBoundingBoxRadius(bbox);
        bbox.scale = 1.f / (2.f * boundingRadius);

        GLContext context(4, 1);
        GLWindow window(800, 800, "scop");

        if (!GLContext::sInitGlad())
        {
            std::cerr << "glad init failed" << std::endl;
            return 1;
        }

        std::vector<unsigned char> vertexSource;
        std::vector<unsigned char> fragmentSource;

        if (!GLUtils::sReadShaderFile("shaders/vertex/source.vert", vertexSource))
            return 1;
        vertexSource.emplace_back('\0');

        if (!GLUtils::sReadShaderFile("shaders/fragment/source.frag", fragmentSource))
            return 1;
        fragmentSource.emplace_back('\0');

        if (vertexSource.empty() || fragmentSource.empty() || vertexSource.size() == 0 || fragmentSource.size() == 0)
        {
            std::cerr << "vertexSource or fragmentSource is empty" << std::endl;
            return 1;
        }

        GLShader shader(reinterpret_cast<char*>(vertexSource.data()), reinterpret_cast<char*>(fragmentSource.data()));
        GLTexture tex("textures/nyan.bmp");

        std::vector<s_vec2> textureCoords;
        if (!tex.generateTexCoord(info.vertices, info.faces, false, textureCoords))
        {
            std::cerr << "failed to generate texture coords" << std::endl;
            return 1;
        }

        std::vector<s_vec3> normals = MathUtils::sComputeVertexNormals(info.vertices, info.faces);

        std::vector<s_Vertex> verticesInterLeaved;
        verticesInterLeaved.reserve(info.vertices.size());
        for (std::size_t i = 0; i < info.vertices.size(); ++i)
        {
            verticesInterLeaved.emplace_back(
                info.vertices[i],
                textureCoords[i],
                normals[i]
            );
        }

        std::vector<s_VertexAttribute> attributes;
        attributes.emplace_back(0, 3, GL_FLOAT, GL_FALSE, sizeof(s_Vertex), offsetof(s_Vertex, position));
        attributes.emplace_back(1, 2, GL_FLOAT, GL_FALSE, sizeof(s_Vertex), offsetof(s_Vertex, texCoord));
        attributes.emplace_back(2, 3, GL_FLOAT, GL_FALSE, sizeof(s_Vertex), offsetof(s_Vertex, normal));

        GLBuffer vbo(GLBuffer::e_Type::Array);
        if (!vbo.setData(verticesInterLeaved, GL_STATIC_DRAW))
           return 1;

        GLMesh mesh;
        if (!mesh.attachVertexBuffer(vbo, attributes))
            return 1;

        GLBuffer ebo(GLBuffer::e_Type::Element);
        if (!ebo.setData(info.faces, GL_STATIC_DRAW))
            return 1;

        if (!mesh.attachElementBuffer(ebo))
            return 1;

        std::vector<s_vec2> textCoordFace;
        if (!tex.generateTexCoord(info.verticesPerFace, info.facesPerFace, true, textCoordFace))
            return 1;

        std::vector<s_vec3> normalsFace = MathUtils::sComputeVertexNormals(info.verticesPerFace, info.facesPerFace);

        std::vector<s_Vertex> verticesInterLeavedFace;
        verticesInterLeavedFace.reserve(info.verticesPerFace.size());
        for (std::size_t i = 0; i < info.verticesPerFace.size(); ++i)
        {
            verticesInterLeavedFace.emplace_back(
                info.verticesPerFace[i],
                textCoordFace[i],
                normalsFace[i]
            );
        }

        GLBuffer vboFace(GLBuffer::e_Type::Array);
        if (!vboFace.setData(verticesInterLeavedFace, GL_STATIC_DRAW))
            return 1;
        
        GLMesh meshFace;
        if (!meshFace.attachVertexBuffer(vboFace, attributes))
            return 1;
        
        GLBuffer eboFace(GLBuffer::e_Type::Element);
        if (!eboFace.setData(info.facesPerFace, GL_STATIC_DRAW))
            return 1;
        if (!meshFace.attachElementBuffer(eboFace))
            return 1;

        s_Transform transform;
        transform.orientation = MathUtils::sQuatIdentify();
        s_renderSettings render;
        s_DisplayInfo displayInfo = {render, transform};

        window.setKeyCallback(smKeyCallback);
        window.setWindowPointer(&displayInfo);
        window.enable(false, true);
        // end of setup

        float fovRadians = MathUtils::sRadiance();
        float distance = MathUtils::sDistance(boundingRadius, fovRadians) * 1.5f;

        s_vec3 up = {0.f, 1.f, 0.f};
        float near = 0.01f;
        float far = 100.f;

        window.setClearColor(0.4f, 0.2f, 0.8f, 1.f);
        GLTimer timer;
        while (!window.shoulClose())
        {
            timer.update();
            GLContext::sPollEvents();
            window.clear();
            window.enable(false, true);
            shader.bind();

            displayInfo.transform.mvp = setupModelViewProjection(fovRadians, near, far, distance, up, window, bbox, displayInfo);

            if (displayInfo.render.useTexture)
                displayInfo.render.blendValue += 1.f * timer.getDeltaTime();
            else
                displayInfo.render.blendValue -= 1.f * timer.getDeltaTime();
            
            displayInfo.render.blendValue = std::clamp(displayInfo.render.blendValue, 0.f, 1.f);

            tex.bind();
            shader.setUniform("uTexture", 0);
            shader.setUniform("uMVP", displayInfo.transform.mvp);
            shader.setUniform("uModel", displayInfo.transform.model);
            shader.setUniform("uBlend", displayInfo.render.blendValue);

            if (displayInfo.render.perFace)
                meshFace.draw(GL_TRIANGLES, info.facesPerFace.size(), GL_UNSIGNED_INT);
            else
                mesh.draw(GL_TRIANGLES, info.faces.size(), GL_UNSIGNED_INT);

            GLenum err = GL_NO_ERROR;
            while ((err = glGetError()) != GL_NO_ERROR)
                std::cerr << "GL error: " << err << std::endl;
            
            window.swapBuffers();
        }

    }
    catch( std::runtime_error& e)
    {
        std::cerr << "runtime error: " << e.what() << std::endl;
        return 1;
    }
    catch( std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}