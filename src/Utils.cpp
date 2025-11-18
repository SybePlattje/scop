#include "Utils.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

s_vec3 Utils::sVec3Normalize(const s_vec3& v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len < 1e-6f)
        return {0.f, 0.f, 0.f};
    return { v.x / len, v.y / len, v.z / len};
}

std::vector<s_vec3> Utils::sComputeVertexNormals
(
    const std::vector<s_vec3>& vertices,
    const std::vector<unsigned int>& indices
)
{
    std::vector<s_vec3> normal(vertices.size(), {0.f, 0.f, 0.f});

    for (std::size_t i = 0; i < indices.size(); i += 3)
    {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        s_vec3 v0 = vertices[i0];
        s_vec3 v1 = vertices[i1];
        s_vec3 v2 = vertices[i2];

        // Calulate the normal of the face
        s_vec3 edge1 = sVec3Subtract(v1, v0);
        s_vec3 edge2 = sVec3Subtract(v2, v0);

        s_vec3 faceNormal = sVec3Normalize(sVec3Cross(edge1, edge2));

        // Accumulate the face normal into each vertex normal
        normal[i0] = sVec3Add(normal[i0], faceNormal);
        normal[i1] = sVec3Add(normal[i1], faceNormal);
        normal[i2] = sVec3Add(normal[i2], faceNormal);
    }

    // Normalize each normal
    for (s_vec3& n : normal)
        n = sVec3Normalize(n);
    
    return normal;
}

s_quat Utils::sQuatIdentify()
{
    s_quat q = {};
    q.w = 1.f;
    q.x = 0.f;
    q.y = 0.f;
    q.z = 0.f;
    return q;
}

s_quat Utils::sQuatNormalize(const s_quat& q)
{
    float len = std::sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    s_quat result = q;
    if (len > 0.f)
    {
        result.w /= len;
        result.x /= len;
        result.y /= len;
        result.z /= len;
    }
    return result;
}

s_BoundingBox Utils::sComputeBoundingBoxAndScale(const std::vector<s_vec3>& vertices)
{
    s_vec3 min = vertices[0];
    s_vec3 max = vertices[0];

    for (const s_vec3& v : vertices)
    {
        min.x = std::min(min.x, v.x);
        min.y = std::min(min.y, v.y);
        min.z = std::min(min.z, v.z);

        max.x = std::max(max.x, v.x);
        max.y = std::max(max.y, v.y);
        max.z = std::max(max.z, v.z);
    }

    s_vec3 center = {
        (min.x + max.x) * 0.5f,
        (min.y + max.y) * 0.5f,
        (min.z + max.z) * 0.5f
    };

    s_vec3 size = sVec3Subtract(max, min);
    float maxDim = std::max({size.x, size.y, size.z});

    float desiredSize = 1.f;
    float scale = desiredSize / maxDim;

    return { min, max, center, size, scale };
}

float Utils::sBoundingBoxRadius(const s_BoundingBox& bbox)
{
    return 0.5f * bbox.scale * 
        std::sqrt(bbox.size.x * bbox.size.x +
            bbox.size.y * bbox.size.y +
            bbox.size.z * bbox.size.z);
}

s_quat Utils::sQuatMultiply(const s_quat& a, const s_quat& b)
{
    s_vec3 v1(a.x, a.y, a.z);
    s_vec3 v2(b.x, b.y, b.z);

    float w = a.w * b.w - sVec3Dot(v1, v2);
    s_vec3 v = v2 * a.w + v1 * b.w + sVec3Cross(v1, v2);
    return {w, v.x, v.y, v.z};
}

s_quat Utils::sQuatFromAxisAngle(const s_vec3& axis, float angleRad)
{
    float halfAngle = angleRad * 0.5f;
    float sinHalfAngle = std::sin(halfAngle);
    return {
        std::cos(halfAngle),
        axis.x * sinHalfAngle,
        axis.y * sinHalfAngle,
        axis.z * sinHalfAngle
    };
}

void Utils::sMat4ToArray(const s_mat4& mat, float* out)
{
    for (int col = 0; col < 4; ++col)
    {
        for (int row = 0; row < 4; ++row)
            out[col * 4 + row] = mat.m[row][col];
    }
}

float Utils::sDistance(float boundingRadius, float fovRadius)
{
    return boundingRadius / std::tan(fovRadius / 3.f);
}

float Utils::sRadiance()
{
    return M_PI / 6.f;
}

s_mat4 Utils::sMat4Multiply(const s_mat4& a, const s_mat4& b)
{
    s_mat4 result = {};

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            result.m[row][col] = 0.f;
            for (int k = 0; k < 4; ++k)
                result.m[row][col] += a.m[row][k] * b.m[k][col];
        }
    }
    return result;
}

s_mat4 Utils::sMat4Perspective(float fovRadian, float aspact, float near, float far)
{
    s_mat4 result = {};
    float localLength = 1.f / std::tan(fovRadian / 2.f);
    result.m[0][0] = localLength / aspact;
    result.m[1][1] = localLength;
    result.m[2][2] = (far + near) / (near - far);
    result.m[2][3] = (2.f * far * near) / (near - far);
    result.m[3][2] = -1.f;
    result.m[3][3] = 0.f;

    return result;
}

s_mat4 Utils::sMat4LookAt(const s_vec3& eye, const s_vec3& center, const s_vec3& worldUp)
{
    s_vec3 forward = sVec3Normalize(sVec3Subtract(center, eye));
    s_vec3 side = sVec3Normalize(sVec3Cross(forward, worldUp));
    s_vec3 camUp = sVec3Normalize(sVec3Cross(side, forward));

    s_mat4 result = sMat4Identify();
    result.m[0][0] = side.x;
    result.m[0][1] = side.y;
    result.m[0][2] = side.z;

    result.m[1][0] = camUp.x;
    result.m[1][1] = camUp.y;
    result.m[1][2] = camUp.z;

    result.m[2][0] = -forward.x;
    result.m[2][1] = -forward.y;
    result.m[2][2] = -forward.z;

    result.m[0][3] = -sVec3Dot(side, eye);
    result.m[1][3] = -sVec3Dot(camUp, eye);
    result.m[2][3] = sVec3Dot(forward, eye);

    return result;
}

s_mat4 Utils::sMat4Scale(float s)
{
    s_mat4 result = {};

    result.m[0][0] = s;
    result.m[1][1] = s;
    result.m[2][2] = s;
    result.m[3][3] = 1.f;

    return result;
}

s_mat4 Utils::sMat4Translate(float tx, float ty, float tz)
{
    s_mat4 mat = sMat4Identify();

    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;

    return mat;
}

s_mat4 Utils::sQuatToMat4(const s_quat& q)
{
    s_mat4 mat = sMat4Identify();

    mat.m[0][0] = 1.f - 2.f * q.y * q.y - 2.f * q.z * q.z; // 1 - 2*y^2 - 2*z^2
    mat.m[0][1] = 2.f * q.x * q.y - 2.f * q.w * q.z; // 2*x*y - 2*w*z
    mat.m[0][2] = 2.f * q.x * q.z + 2.f * q.w * q.y; // 2*x*z + 2*w*y
    mat.m[0][3] = 0.f;

    mat.m[1][0] = 2.f * q.x * q.y + 2.f * q.w *q.z; // 2*x*y + 2*w*z
    mat.m[1][1] = 1.f - 2.f * q.x * q.x - 2.f * q.z * q.z; // 1 - 2*2^2 - 2*z^2
    mat.m[1][2] = 2.f * q.y * q.z - 2.f * q.w * q.x; // 2*y*z - 2*w*x
    mat.m[1][3] = 0.f;

    mat.m[2][0] = 2.f * q.x * q.z - 2.f * q.w * q.y; // 2*x*z - 2*w*y
    mat.m[2][1] = 2.f * q.y * q.z + 2.f * q.w * q.x; // 2*y*z + 2*w*x
    mat.m[2][2] = 1.f - 2.f * q.x * q.x - 2.f * q.y * q.y; // 1 - 2*x^2 - 2*y^2
    mat.m[2][3] = 0.f;

    mat.m[3][0] = 0.f;
    mat.m[3][1] = 0.f;
    mat.m[3][2] = 0.f;
    mat.m[3][3] = 1.f;

    return mat;
}

s_mat4 Utils::sMat4Identify()
{
    s_mat4 result = {};

    for (int i = 0; i < 4; ++ i)
        result.m[i][i] = 1.f;
    return result;
}

s_vec3 Utils::sVec3Subtract(const s_vec3& a, const s_vec3& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

s_vec3 Utils::sVec3Add(const s_vec3& a, const s_vec3& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

s_vec3 Utils::sVec3Cross(const s_vec3& a, const s_vec3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float Utils::sVec3Dot(const s_vec3& a, const s_vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

s_InputFileLines Utils::sParseInput(const char* path)
{
    if (!path)
        throw std::runtime_error("path cannot be empty");

    std::filesystem::path filePath(path);
    if (".obj" != filePath.extension())
        throw std::runtime_error("file needs to be a .obj");

    std::ifstream fstream(path);
    if (!fstream)
    {
        std::string fileString = "resources/" + static_cast<std::string>(path);
        fstream.open(fileString);
        if (!fstream)
            throw std::runtime_error("File not found");
    }

    s_InputFileLines result;

    std::string line;
    result.faces.reserve(3000);
    result.facesPerFace.reserve(3000);
    result.vertices.reserve(3000);
    result.verticesPerFace.reserve(3000);
    while (std::getline(fstream, line))
    {
        if (0 == line.rfind("v ", 0))
        {
            float x, y, z;
            std::sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
            result.vertices.emplace_back(x, y, z);
        }
        else if (0 == line.rfind("f ", 0))
        {
            std::vector<unsigned int> faceIndices;
            faceIndices.reserve(3000);
            unsigned int id;
            std::stringstream ss(line);
            std::string token;
            ss >> token;
            while (ss >> id)
                faceIndices.emplace_back(id - 1);

            for (std::size_t i = 1; i < faceIndices.size() - 1; ++i)
            {
                result.faces.emplace_back(faceIndices[0]);
                result.faces.emplace_back(faceIndices[i]);
                result.faces.emplace_back(faceIndices[i + 1]);
            }
        }
    }

    for (std::size_t i = 0; i < result.faces.size(); i += 3)
    {
        for (int j = 0; j < 3; ++j)
        {
            unsigned int vertexIndex = result.faces[i + j];
            s_vec3 vertex = result.vertices[vertexIndex];

            result.verticesPerFace.emplace_back(vertex);

            result.facesPerFace.emplace_back(static_cast<unsigned int>(result.facesPerFace.size()));
        }
    }

    if (0 == result.vertices.size() || 0 == result.faces.size())
        throw std::runtime_error("no vertices or faces found in file");

    return result;
}

s_vec3 operator*(const s_vec3& v, float x)
{
    return {v.x + x, v.y + x, v.z + x};
}

s_vec3 operator+(const s_vec3& a, const s_vec3& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}