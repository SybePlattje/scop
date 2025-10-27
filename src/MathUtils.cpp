#include "MathUtils.hpp"
#include <algorithm>
#include <cmath>

s_vec3 MathUtils::sVec3Normalize(const s_vec3& v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len < 1e-6f)
        return {0.f, 0.f, 0.f};
    return { v.x / len, v.y / len, v.z / len};
}

std::vector<s_vec3> MathUtils::sComputeVertexNormals
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
        s_vec3 edge1 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
        s_vec3 edge2 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};

        s_vec3 faceNormal = sVec3Normalize({
            edge1.y * edge2.z - edge1.z * edge2.y,
            edge1.z * edge2.x - edge1.x * edge2.z,
            edge1.x * edge2.y - edge1.y * edge2.x
        });

        // Accumulate the face normal into each vertex mormal
        normal[i0].x += faceNormal.x;
        normal[i0].y += faceNormal.y;
        normal[i0].z += faceNormal.z;

        normal[i1].x += faceNormal.x;
        normal[i1].y += faceNormal.y;
        normal[i1].z += faceNormal.z;

        normal[i2].x += faceNormal.x;
        normal[i2].y += faceNormal.y;
        normal[i2].z += faceNormal.z;
    }

    // Normalize each normal
    for (s_vec3& n : normal)
        n = sVec3Normalize(n);
    
    return normal;
}

s_quat MathUtils::sQuatIdentify()
{
    s_quat q = {};
    q.w = 1.f;
    q.x = 0.f;
    q.y = 0.f;
    q.z = 0.f;
    return q;
}

s_quat MathUtils::sQuatNormalize(const s_quat& q)
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

s_BoundingBox MathUtils::sComputeBoundingBoxAndScale(const std::vector<s_vec3>& vertices)
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

    s_vec3 size = {max.x - min.x, max.y - min.y, max.z - min.z};
    float maxDim = std::max({size.x, size.y, size.z});

    float desiredSize = 1.f;
    float scale = desiredSize / maxDim;

    return { min, max, center, size, scale };
}

float MathUtils::sBoundingBoxRadius(const s_BoundingBox& bbox)
{
    return 0.5f * bbox.scale * 
        std::sqrt(bbox.size.x * bbox.size.x +
            bbox.size.y * bbox.size.y +
            bbox.size.z * bbox.size.z);
}

s_quat MathUtils::sQuatMultiply(const s_quat& a, const s_quat& b)
{
    return {
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
        a.w * b.x + a.x * b.w + a.y * b.z + a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w
    };
}

s_quat MathUtils::sQuatFromAxisAngle(const s_vec3& axis, float angleRad)
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

void MathUtils::sMat4ToArray(const s_mat4& mat, float* out)
{
    for (int col = 0; col < 4; ++col)
    {
        for (int row = 0; row < 4; ++row)
            out[col * 4 + row] = mat.m[row][col];
    }
}

float MathUtils::sDistance(float boundingRadius, float fovRadius)
{
    return boundingRadius / std::tan(fovRadius / 2.f);
}

float MathUtils::sRadiance()
{
    return M_PI / 5.f;
}

s_mat4 MathUtils::sMat4Multiply(const s_mat4& a, const s_mat4& b)
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

s_mat4 MathUtils::sMat4Perspective(float fovRadian, float aspact, float near, float far)
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

s_mat4 MathUtils::sMat4LookAt(const s_vec3& eye, const s_vec3& center, const s_vec3& worldUp)
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

s_mat4 MathUtils::sMat4Scale(float s)
{
    s_mat4 result = {};

    result.m[0][0] = s;
    result.m[1][1] = s;
    result.m[2][2] = s;
    result.m[3][3] = 1.f;

    return result;
}

s_mat4 MathUtils::sMat4Translate(float tx, float ty, float tz)
{
    s_mat4 mat = sMat4Identify();

    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;

    return mat;
}

s_mat4 MathUtils::sQuatToMat4(const s_quat& q)
{
    s_mat4 mat = sMat4Identify();

    mat.m[0][0] = 1.f - 2.f * q.y * q.y - 2.f * q.z * q.z;
    mat.m[0][1] = 2.f * q.x * q.y - 2.f * q.w * q.z;
    mat.m[0][2] = 2.f * q.x * q.z + 2.f * q.w * q.y;
    mat.m[0][3] = 0.f;

    mat.m[1][0] = 2.f * q.x * q.y + 2.f * q.w *q.z;
    mat.m[1][1] = 1.f - 2.f * q.x * q.x - 2.f * q.z * q.z;
    mat.m[1][2] = 2.f * q.y * q.z - 2.f * q.w * q.x;
    mat.m[1][3] = 0.f;

    mat.m[2][0] = 2.f * q.x * q.z - 2.f * q.w * q.y;
    mat.m[2][1] = 2.f * q.y * q.z + 2.f * q.w * q.x;
    mat.m[2][2] = 1.f - 2.f * q.x * q.x - 2.f * q.y * q.y;
    mat.m[2][3] = 0.f;

    mat.m[3][0] = 0.f;
    mat.m[3][1] = 0.f;
    mat.m[3][2] = 0.f;
    mat.m[3][3] = 1.f;

    return mat;
}

s_mat4 MathUtils::sMat4Identify()
{
    s_mat4 result = {};

    for (int i = 0; i < 4; ++ i)
        result.m[i][i] = 1.f;
    return result;
}

s_vec3 MathUtils::sVec3Subtract(const s_vec3& a, const s_vec3& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

s_vec3 MathUtils::sVec3Cross(const s_vec3& a, const s_vec3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float MathUtils::sVec3Dot(const s_vec3& a, const s_vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}