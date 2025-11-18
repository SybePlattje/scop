#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include "GLShader.hpp"
# include "Struct.hpp"

class Utils
{
	public:
		static std::vector<s_vec3> sComputeVertexNormals(const std::vector<s_vec3>& vertices, const std::vector<unsigned int>& indices);
		static s_BoundingBox sComputeBoundingBoxAndScale(const std::vector<s_vec3>& vertices);
		static s_vec3 sVec3Normalize(const s_vec3& v);
		static s_vec3 sVec3Subtract(const s_vec3& a, const s_vec3& b);
        static s_vec3 sVec3Add(const s_vec3& a, const s_vec3& b);
		static s_vec3 sVec3Cross(const s_vec3& a, const s_vec3& b);
		static float sVec3Dot(const s_vec3& a, const s_vec3& b);
		static s_quat sQuatIdentify();
		static s_quat sQuatMultiply(const s_quat& a, const s_quat& b);
		static s_quat sQuatFromAxisAngle(const s_vec3& axis, float angleRad);
		static s_quat sQuatNormalize(const s_quat& q);
		static void sMat4ToArray(const s_mat4& mat, float* out);
		static float sBoundingBoxRadius(const s_BoundingBox& bbox);
		static float sDistance(float boundingRadius, float fovRadius);
		static float sRadiance();
		static s_mat4 sMat4Multiply(const s_mat4& a, const s_mat4&b);
		static s_mat4 sMat4Perspective(float fovRadian, float aspact, float near, float far);
		static s_mat4 sMat4LookAt(const s_vec3& eye, const s_vec3& center, const s_vec3& worldUp);
		static s_mat4 sMat4Scale(float s);
		static s_mat4 sMat4Translate(float tx, float ty, float tz);
		static s_mat4 sQuatToMat4(const s_quat& q);
		static s_mat4 sMat4Identify();
		static s_InputFileLines sParseInput(const char* path);
};

#endif