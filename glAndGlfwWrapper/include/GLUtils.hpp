#ifndef GLUTILS_HPP
# define GLUTILS_HPP

# include <string>
# include <vector>

class GLUtils
{
    public:
        static bool sReadShaderFile(const char* path, std::vector<unsigned char>& buffer);
        static bool sReadTexture(const char* path, std::vector<unsigned char>& buffer);
};

#endif