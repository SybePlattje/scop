#ifndef GLSHADER_HPP
# define GLSHADER_HPP

# include <string>
# include <glad/glad.h>
# include <type_traits>
# include <unordered_map>
# include <vector>

struct s_vec2 { float x, y; };
struct s_vec3 { float x, y, z; };
struct s_vec4 { float x, y, z, w; };
struct s_mat4 { float m[4][4]; };
struct s_quat { float x, y, z, w; };

template<typename T> struct is_vec2 : std::false_type {};
template<typename T> struct is_vec3 : std::false_type {};
template<typename T> struct is_vec4 : std::false_type {};
template<typename T> struct is_mat4 : std::false_type {};
template<typename T> struct is_quat : std::false_type {};

template<> struct is_vec2<s_vec2> : std::true_type {};
template<> struct is_vec3<s_vec3> : std::true_type {};
template<> struct is_vec4<s_vec4> : std::true_type {};
template<> struct is_mat4<s_mat4> : std::true_type {};
template<> struct is_quat<s_quat> : std::true_type {};

template<class> struct always_false : std::false_type {};

class GLShader
{
    public:
        GLShader(const std::string& vertexSource, const std::string& fragmentSource);
        ~GLShader();

        void bind() const;
        void unbind() const;
        GLuint getProgramId() const;
        GLuint compileShader(GLenum type, const std::string& source);
        bool linkProgram();
        void attachShader(GLuint shader);
        
        static std::string sShaderTypeToString(GLenum type);

        /**
         * @param name the name of the uniform
         * @param value the  values the uniform will get
         * @brief while being checked on type we try to set the value of the uniform
         * @warning static_assert will check if passed values are support
         */
        template<typename T>
        void setUniform(const std::string& name, const T& value)
        {
            GLint location = getUniformLocation(name);

            if constexpr (std::is_same_v<T, int>)
                glUniform1i(location, value);
            else if constexpr (std::is_same_v<T , float>)
                glUniform1f(location, value);
            else if constexpr (is_vec2<T>::value)
                glUniform2f(location, value.x, value.y);
            else if constexpr (is_vec3<T>::value)
                glUniform3f(location, value.x, value.y, value.z);
            else if constexpr (is_vec4<T>::value)
                glUniform4f(location, value.x, value.y, value.z, value.w);
            else if constexpr (is_mat4<T>::value)
                glUniformMatrix4fv(location, 1, GL_FALSE, &value.m[0][0]);
            else if constexpr (is_quat<T>::value)
                glUniform4f(location, value.x, value.y, value.z, value.w);
            else
                static_assert(always_false<T>::value, "Unsupported uniform type");
        }
    private:
        GLuint m_program;
        std::unordered_map<std::string, GLint> m_uniformCache;
        std::vector<GLuint> m_shaders;

        GLint getUniformLocation(const std::string& name);
        bool checkCompileErrors(GLuint object, GLenum type, bool isProgram);
};

#endif