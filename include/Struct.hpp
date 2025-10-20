#ifndef STRUCT_HPP
# define STRUCT_HPP

# include "GLShader.hpp"

struct s_Transform
{
	s_quat rotation;
	s_quat orientation;
	s_mat4 model;
	s_mat4 mvp;
	float zoomFactor = 1.f;
	const float zoomStep = 0.1f;
	const float minZoom = 0.2f;
	const float maxZoom = 5.f;
};

struct s_BoundingBox
{
	s_vec3 min;
	s_vec3 max;
	s_vec3 center;
	s_vec3 size;
	float scale;
};

struct s_renderSettings
{
	bool useTexture = false;
	float blendValue = 0.f;
	bool perFace = false;
};

struct s_DisplayInfo
{
	s_renderSettings render;
	s_Transform transform;
};

struct s_Vertex
{
	s_vec3 position;
	s_vec2 texCoord;
	s_vec3 normal;
};


#endif